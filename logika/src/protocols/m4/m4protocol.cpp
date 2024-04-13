/// @file Реализация протокола M4
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/m4protocol.h>

#include <logika/log/defines.h>

#include <logika/protocols/comm_exception.h>
#include <logika/protocols/m4/opcodes.h>
#include <logika/connections/utils/types_converter.h>
#include <logika/connections/serial/serial_connection.h>
#include <logika/meters/logika4/4m/logika4m.h>
#include <logika/common/misc.h>

/// @cond
#include <array>
#include <algorithm>
/// @endcond

namespace // anonymous
{

/// @brief Попытка приведения соединения к SerialConnection
/// @param[in] connection Соединение
/// @return Указатель на соединение, если оно Serial. nullptr иначе
/// @throws std::logical_error если тип соединения указан как serial,
/// но при этом соединение не является SerialConnection
std::shared_ptr< logika::connections::SerialConnection > ConnectionAsSerial(
    std::shared_ptr< logika::connections::IConnection > connection )
{
    if ( !connection
        || ( connection->GetConnectionType() & logika::connections::ConnectionType::Serial ) )
    {
        return nullptr;
    }
    std::shared_ptr< logika::connections::SerialConnection > serialConnection
        = std::dynamic_pointer_cast< logika::connections::SerialConnection >( connection );
    if ( !serialConnection )
    {
        throw std::logic_error{ "Invalid connection type" };
    }
    return serialConnection;
} // ConnectionAsSerial

} // anonymous namespace


namespace logika
{

namespace protocols
{

namespace M4
{

const ByteType M4Protocol::FRAME_START              = static_cast< ByteType >( 0x10 );
const ByteType M4Protocol::FRAME_END                = static_cast< ByteType >( 0x16 );
const ByteType M4Protocol::EXT_PROTO                = static_cast< ByteType >( 0x90 );
const ByteType M4Protocol::BROADCAST_NT             = static_cast< ByteType >( 0xFF );
const uint32_t M4Protocol::MAX_RAM_REQUEST          = 0x40;
const uint32_t M4Protocol::MAX_TAGS_AT_ONCE         = 24;
const uint16_t M4Protocol::PARTITION_CURRENT        = 0xFFFF;
const uint32_t M4Protocol::ALT_SPEED_FALLBACK_TIME  = 10000;
/// Последовательность состоит из 16 байтов 0xFF
const ByteVector M4Protocol::WAKEUP_SEQUENCE        = ByteVector( 16, static_cast< ByteType >( 0xFF ) );
const TimeType M4Protocol::WAKE_SESSION_DELAY       = 100;
uint32_t M4Protocol::packetId_                      = 0;


M4Protocol::BusActiveState::BusActiveState( std::shared_ptr< meters::Logika4 > mtr,
    ByteType ntByte, MeterChannel::Type chan )
    : meter{ mtr }
    , nt{ ntByte }
    , tv{ chan }
    , lastIo{ 0 }
    , hasIoError{ false }
{
    if ( !meter )
    {
        throw std::invalid_argument{ "Invalid meter passed" };
    }
} // BusActiveState


TimeType M4Protocol::BusActiveState::GetTimeFromLastIo() const
{
    return GetCurrentTimestamp() - lastIo;
} // GetTimeFromLastIo


M4Protocol::M4Protocol( connections::BaudRate::Type targetBaudRate )
    : Protocol()
    , initialBaudRate_{ connections::BaudRate::NotSupported }
    , suggestedBaudRate_{ targetBaudRate }
    , activeDev_{ nullptr }
{} // M4Protocol


connections::BaudRate::Type M4Protocol::GetCurrentBaudRate() const
{
    std::shared_ptr< connections::SerialConnection > serialConnection = ConnectionAsSerial( connection_ );
    if ( !serialConnection )
    {
        return connections::BaudRate::NotSupported;
    }
    return serialConnection->GetBaudRate();
} // GetCurrentBaudRate


void M4Protocol::ResetBusActiveState()
{
    activeDev_.reset();
    SerialSpeedFallback();
    LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "M4 bus state is reset" ) );
} // ResetBusActiveState


void M4Protocol::SendAttention( bool slowWake )
{
    constexpr TimeType slowWakeTimeout = 20; /// 20 мс между отправками

    if ( !connection_ )
    {
        return;
    }
    
    connection_->Purge( connections::PurgeFlags::TxRx );
    if ( !slowWake )
    {
        connection_->Write( WAKEUP_SEQUENCE );
        return;
    }

    for ( size_t i = 0; i < WAKEUP_SEQUENCE.size(); ++i )
    {
        connection_->Write( ByteVector{ WAKEUP_SEQUENCE.at( i ) } );
        WaitFor( slowWakeTimeout );
    }
} // SendAttention


void M4Protocol::SelectDeviceAndChannel( std::shared_ptr< meters::Logika4 > meter,
    const storage::StorageKeeper& sKeeper, ByteType* zNt, MeterChannel::Type tv )
{
    if ( !connection_ )
    {
        throw ECommException( ExceptionSeverity::Stop,
            CommunicationError::NotConnected, "Connection not established" );
    }
    if ( !meter )
    {
        throw std::invalid_argument{ "Invalid meter passed" };
    }
    ByteType nt = zNt ? *zNt : M4Protocol::BROADCAST_NT;

    /// @note activeDev_->meter проверяется в конструкторе BusActiveState
    /// Проверяем, не закончилась ли сессия в приборе по неактивности
    if ( activeDev_ && activeDev_->GetTimeFromLastIo() > activeDev_->meter->GetSessionTimeout() )
    {
        ResetBusActiveState();
    }
    std::shared_ptr< connections::SerialConnection > serialConnection = ConnectionAsSerial( connection_ );
    if ( serialConnection )
    {
        /// Менеджмент повышенных скоростей для serial соединений
        if ( suggestedBaudRate_ != connections::BaudRate::NotSupported
            && initialBaudRate_ == connections::BaudRate::NotSupported )
        {
            initialBaudRate_ = serialConnection->GetBaudRate();
        }
        /// Проверяем, не вывалился ли прибор из повышенной скорости
        /// (прибор сбрасывает скорость обмена при отсутствии обмена в течение 10 с )
        if ( activeDev_ && activeDev_->GetTimeFromLastIo() > M4Protocol::ALT_SPEED_FALLBACK_TIME )
        {
            SerialSpeedFallback();
        }
        /// Если внутреннее состояние не задано или не совпадает с ожидаемым,
        /// или произошла ошибка ввода/вывода, то нужно его переинициализровать
        bool needReselect = !activeDev_ || activeDev_->nt != nt
            || activeDev_->tv != tv || activeDev_->hasIoError;
        if ( needReselect )
        {
            if ( activeDev_ )
            {
                activeDev_->hasIoError = false;
            }
            bool alreadyAwake = activeDev_ && activeDev_->nt == nt;
            /// Выдерживаем паузы между FFами и после них, только если устройство этого требует
            bool slowWake = !meter->GetSupportFastSessionInit() && !alreadyAwake;
            try
            {
                Packet handshakePacket = DoHandshake( &nt, static_cast< ByteType >( tv ), slowWake );
                /// @note connection_ не nullptr, проверено выше, поэтому Packet не пуст
                std::shared_ptr< meters::Meter > detectedMtr = meters::Logika4::DetermineMeter(
                    handshakePacket.data.at( 0 ), handshakePacket.data.at( 1 ), handshakePacket.data.at( 2 ),
                    sKeeper.GetStorage< LocString, meters::Meter >()
                );
                if ( detectedMtr != meter )
                {
                    ResetBusActiveState();
                    throw std::runtime_error{ "Invalid meter type detected" };
                }
                activeDev_ = std::make_shared< BusActiveState >( meter, nt, tv );
                activeDev_->lastIo = GetCurrentTimestamp();
            }
            catch ( const std::exception& )
            {
                LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "An error occured due handshake and meter check" ) );
                activeDev_->hasIoError = true;
                throw;
            }
        }
        /// Обновление скорости работы прибора
        while ( true )
        {
            if ( serialConnection
                && suggestedBaudRate_ != connections::BaudRate::NotSupported
                && GetCurrentBaudRate() != suggestedBaudRate_ )
            {
                bool updatedSpeed = SetBusSpeed( activeDev_->meter, &nt, suggestedBaudRate_, tv );
                if ( !updatedSpeed )
                {
                    /// У некоторых M4 приборов (941.20, 944, 742? прочие?) встречается несовместимость,
                    /// не позволяющая нормально работать на максимальной скорости 57600 через АПС71
                    std::shared_ptr< meters::Logika4M > meter4M =
                        std::dynamic_pointer_cast< meters::Logika4M >( meter );
                    if ( meter4M && suggestedBaudRate_ >= connections::BaudRate::Rate57600 )
                    {
                        suggestedBaudRate_ = connections::BaudRate::Rate38400;
                        continue;
                    }
                    /// Также, 942 поддерживает 9600 только на оптическом интерфейсе, на проводном - только 2400
                    suggestedBaudRate_ = connections::BaudRate::NotSupported;
                }
            }
            break;
        }
    }
} // SelectDeviceAndChannel


ByteVector M4Protocol::GenerateRawHandshake( ByteType* dstNt )
{
    static const ByteVector hsArgs{ 0, 0, 0, 0 };
    ByteVector packet;
    packet.resize( 3 + hsArgs.size() + 2 );
    packet[ 0 ] = M4Protocol::FRAME_START;
    packet[ 1 ] = dstNt ? *dstNt : M4Protocol::BROADCAST_NT;
    packet[ 2 ] = Opcode::Handshake;
    for ( size_t i = 0; i < hsArgs.size(); ++i )
    {
        packet[ 3 + i ] = hsArgs.at( i );
    }
    packet[ 3 + hsArgs.size() ] = meters::Logika4::CheckSum8( packet, 1, hsArgs.size() + 2 );
    packet[ 3 + hsArgs.size() + 1 ] = M4Protocol::FRAME_END;
    return packet;
} // GenerateRawHandshake


Packet M4Protocol::RecvPacket( ByteType* excpectedNt, Opcode::Type* expectedOpcode, ByteType* expectedId,
    uint32_t expectedDataLength, RecvFlags::Type flags )
{
    ByteVector buffer;
    ByteVector check;
    uint32_t readed     = 0;
    uint32_t payloadLen = 0;

    Packet packet{};
    if ( !connection_ )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Connection not established" ) );
        return packet;
    }

    try
    {
        while ( true ) // Чтение пакетов
        {
            TimeType readStart = GetCurrentTimestamp();
            while ( true ) /// Чтение пока не будет обнаружено начало фрейма или не исчеет таймаут
            {
                buffer.clear();
                readed = connection_->Read( buffer, 1 );
                if ( readed == 1 && buffer.at( 0 ) == M4Protocol::FRAME_START )
                {
                    break; // Считано начало фрейма
                }
                TimeType elapsed = GetCurrentTimestamp() - readStart;
                if ( elapsed > connection_->GetReadTimeout() )
                {
                    OnRecoverableError();
                    throw ECommException( ExceptionSeverity::Error,
                        CommunicationError::Timeout, "Timeout expired" );
                }
            }
            /// Чтение NT и кода операции
            readed = connection_->Read( buffer, 2 );
            if ( readed < 2 )
            {
                OnRecoverableError();
                throw ECommException( ExceptionSeverity::Error,
                    CommunicationError::Unspecified, "Communication sequence error" );
            }
            packet.nt           = buffer.at( 1 );
            packet.funcOpcode   = static_cast< Opcode::Type >( buffer.at( 2 ) );
            packet.data         = ByteVector{};
            /// Это может быть как ответ другого прибора (очень маловероятно),
            /// так и случайная синхронизация по 0x10 в потоке данных
            if ( excpectedNt && packet.nt != *excpectedNt )
            {
                continue; /// Чтение следующего пакета
            }
            /// Либо задержавшийся в буферах устройств связи ответ на предыдущий запрос(наиболее вероятно),
            /// либо случайно случившиеся в потоке 10 NT(маловероятно),
            /// либо ответ другого прибора(крайне маловероятно)
            if (   expectedOpcode
                && static_cast< Opcode::Type >( packet.funcOpcode ) != *expectedOpcode
                && static_cast< Opcode::Type >( packet.funcOpcode ) != Opcode::Error
                && packet.funcOpcode != M4Protocol::EXT_PROTO )
            {
                /// Для многостраничного чтения Flash ошибки в приеме
                /// последовательности пакетов недопустимы, поэтому никаких переприёмов.
                if ( static_cast< Opcode::Type >( packet.funcOpcode ) == Opcode::ReadFlash )
                {
                    OnRecoverableError();
                    throw ECommException( ExceptionSeverity::Error,
                        CommunicationError::Unspecified, "Communication sequence error" );
                }
                continue; /// Чтение следующего пакета
            }

            if ( packet.funcOpcode == M4Protocol::EXT_PROTO )
            {
                /// Extended протокол
                packet.extended = true;
                readed = connection_->Read( buffer, 5 );
                if ( readed < 5 )
                {
                    OnRecoverableError();
                    /// @bug Сделать повторное чтение?
                    throw ECommException( ExceptionSeverity::Error,
                        CommunicationError::SystemError, "General read error" );
                }
                packet.id           = buffer.at( 3 );
                packet.attributes   = buffer.at( 4 );
                /// payload = opcode + data
                payloadLen          = buffer.at( 5 ) + ( buffer.at( 6 ) << 8 ) - 1;
                packet.funcOpcode   = static_cast< Opcode::Type >( buffer.at( 7 ) );
                if ( expectedOpcode && packet.funcOpcode != *expectedOpcode )
                {
                    continue; /// Чтение следующего пакета
                }
                if ( expectedId && packet.id != *expectedId )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Invalid comm sequence: expected ID 0x"
                        << std::hex << *expectedId << ", got 0x" << std::hex << packet.id ) );
                    continue; /// Чтение следующего пакета
                }
            }
            else
            {
                /// Стандартный протокол
                packet.extended = false;
                if ( static_cast< Opcode::Type >( packet.funcOpcode ) == Opcode::Error )
                {
                    /// Данные состоят только из кода ошибки
                    payloadLen = 1;
                }
                else
                {
                    payloadLen = expectedDataLength;
                }
            }
            /// data + CSUM + frame end
            readed = connection_->Read( packet.data, payloadLen );
            if ( readed < payloadLen )
            {
                OnRecoverableError();
                /// @bug Сделать повторное чтение?
                throw ECommException( ExceptionSeverity::Error,
                    CommunicationError::SystemError, "General read error" );
            }
            /// legacy: checksum8 + frame end, M4: CRC16
            readed = connection_->Read( check, 2 );
            if ( readed < 2 )
            {
                OnRecoverableError();
                /// @bug Сделать повторное чтение?
                throw ECommException( ExceptionSeverity::Error,
                    CommunicationError::SystemError, "General read error" );
            }
            if ( packet.extended )
            {
                /// MSB first
                packet.checkSum = ( static_cast< uint16_t >( check.at( 0 ) ) << 8 ) | check.at( 1 );
            }
            else
            {
                // Используется доп проверка END_OF_FRAME (0x16)
                packet.checkSum = check.at( 0 ) | ( static_cast< uint16_t >( check.at( 1 ) ) << 8 );
            }
            if ( activeDev_ )
            {
                activeDev_->lastIo = GetCurrentTimestamp();
            }
            break; // Пакет считан
        }
    }
    catch( const std::exception& e )
    {
        OnRecoverableError();
        throw;
    }

    uint16_t calculatedChecksum = 0;
    if ( packet.extended )
    {
        /// Расчет CRC16
        calculatedChecksum = Protocol::Crc16( calculatedChecksum, buffer, 1, 7 );
        calculatedChecksum = Protocol::Crc16( calculatedChecksum, packet.data, 0, packet.data.size() );
    }
    else
    {
        /// Расчет Checksum8
        calculatedChecksum = 0x1600; /// END_OF_FRAME << 8
        /// header + data
        calculatedChecksum |= static_cast< ByteType >(
            ~meters::Logika4::CheckSum8( buffer, 1, 2 ) +
            ~meters::Logika4::CheckSum8( packet.data, 0, packet.data.size() )
        );
        if ( packet.checkSum != calculatedChecksum )
        {
            /// Несовпадение контрольной суммы
            RegisterEvent( Rc::RxCrcError );
            throw ECommException( ExceptionSeverity::Error, CommunicationError::Checksum );
        }
        RegisterEvent( Rc::PacketReceived );
        if ( static_cast< Opcode::Type >( packet.funcOpcode ) == Opcode::Error )
        {
            Rc::Type errorCode = static_cast< Rc::Type >( packet.data.at( 0 ) );
            RegisterEvent( Rc::RxGeneralError );
            if ( ( flags & RecvFlags::DontThrowOnErrorReply ) == 0 )
            {
                throw ECommException( ExceptionSeverity::Error, CommunicationError::Unspecified,
                    "", LOCALIZED( "Device returned error: " ) + ToLocString( std::to_string( errorCode ) ) );
            }
        }
    }

    return packet;
} // RecvPacket


void M4Protocol::SendLegacyPacket( ByteType* nt, Opcode::Type func, const ByteVector& data )
{
    if ( !connection_ )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Connection not established" ) );
        return;
    }

    /// @note Пакет формируется в виде сырого массива байтов
    ByteVector buffer( 3 + data.size() + 2, 0x0 );
    buffer[ 0 ] = M4Protocol::FRAME_START;
    buffer[ 1 ] = nt ? *nt : M4Protocol::BROADCAST_NT;
    buffer[ 2 ] = static_cast< ByteType >( func );
    for ( size_t i = 0; i < data.size(); ++i )
    {
        buffer[ 3 + i ] = data.at( i );
    }
    buffer[ 3 + data.size() ] = meters::Logika4::CheckSum8( buffer, 1, data.size() + 2 );
    buffer[ 3 + data.size() + 1 ] = M4Protocol::FRAME_END;

    if ( 0 != connection_->Write( buffer ) )
    {
        RegisterEvent( Rc::PacketTransmitted );
    }
    else
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Legacy packet send error" ) );
    }
} // SendLegacyPacket


void M4Protocol::SendExtendedPacket( ByteType* nt, ByteType packetId, Opcode::Type opcode, const ByteVector& data )
{
    constexpr uint32_t headerLen    = 8;
    constexpr uint32_t crcLen       = 2;

    if ( !connection_ )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Connection not established" ) );
        return;
    }
    /// @note Пакет формируется в виде сырого массива байтов
    ByteVector buffer( headerLen + data.size() + crcLen, 0x0 );

    buffer[ 0 ] = M4Protocol::FRAME_START;
    buffer[ 1 ] = nt ? *nt : M4Protocol::BROADCAST_NT;
    buffer[ 2 ] = M4Protocol::EXT_PROTO; /// Идентификатор расширенной версии протокола
    buffer[ 3 ] = packetId; /// Идентификатор пакета
    buffer[ 4 ] = 0x00; /// Атрибуты пакета

    /// opcode + data
    const uint32_t payloadLen = static_cast< uint16_t >( data.size() + 1 );
    /// Размер данных: 2 байта
    buffer[ 5 ] = static_cast< ByteType >( payloadLen & 0xFF );
    buffer[ 6 ] = static_cast< ByteType >( payloadLen >> 8 );
    buffer[ 7 ] = static_cast< ByteType >( opcode );
    /// Данные пакета
    for ( size_t i = 0; i < data.size(); ++i )
    {
        buffer[ headerLen + i ] = data.at( i );
    }
    /// Расчет контрольной суммы CRC16
    uint16_t checkSum = 0;
    checkSum = Protocol::Crc16( checkSum, buffer, 1, headerLen - 1 + data.size() );
    buffer[ headerLen + data.size() ] = static_cast< ByteType >( checkSum >> 8 );
    buffer[ headerLen + data.size() + 1 ] = static_cast< ByteType >( checkSum & 0xFF );

    if ( 0 != connection_->Write( buffer ) )
    {
        RegisterEvent( Rc::PacketTransmitted );
    }
    else
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Legacy packet send error" ) );
    }
} // SendExtendedPacket


Packet M4Protocol::DoLegacyRequest( ByteType* nt, Opcode::Type requestOpcode, const ByteVector& data,
    uint32_t expectedDataLength, RecvFlags::Type flags )
{
    SendLegacyPacket( nt, requestOpcode, data );
    return RecvPacket( nt, &requestOpcode, nullptr, expectedDataLength, flags );
} // DoLegacyRequest


Packet M4Protocol::DoM4Request( ByteType* nt, Opcode::Type requestOpcode, const ByteVector& data,
    ByteType* packetId, RecvFlags::Type flags )
{
    ByteType identifier;
    if ( nt )
    {
        identifier = *nt;
    }
    else
    {
        identifier = static_cast< ByteType >( packetId_++ );
    }
    SendExtendedPacket( nt, identifier, requestOpcode, data );
    return RecvPacket( nt, &requestOpcode, &identifier, 0, flags );
} // DoM4Request


Packet M4Protocol::DoHandshake( ByteType* nt, ByteType channel, bool slowWake )
{
    if ( !connection_ )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Connection not established" ) );
        return Packet{};
    }
    if ( activeDev_ && nt && activeDev_->nt == *nt )
    {
        ResetBusActiveState();
    }
    SendAttention( slowWake );
    /// Пауза обязательна для АДС99 в режиме TCP сервер,
    /// без неё он всячески таймаутится, остальным приборам +/- всё равно
    WaitFor( M4Protocol::WAKE_SESSION_DELAY );
    connection_->Purge( connections::PurgeFlags::Rx );
    const ByteVector requestData{ channel, 0, 0, 0 };
    return DoLegacyRequest( nt, Opcode::Handshake, requestData, 3 );
} // DoHandshake


bool M4Protocol::SetBusSpeed( std::shared_ptr< meters::Logika4 > meter, ByteType* nt,
    connections::BaudRate::Type baudRate, MeterChannel::Type tv )
{
    static const std::array< connections::BaudRate::Type, 7 > m4BaudRates{
          connections::BaudRate::Rate2400, connections::BaudRate::Rate4800
        , connections::BaudRate::Rate9600, connections::BaudRate::Rate19200
        , connections::BaudRate::Rate38400, connections::BaudRate::Rate57600
        , connections::BaudRate::Rate115200
    };

    std::shared_ptr< connections::SerialConnection > serialConnection = ConnectionAsSerial( connection_ );
    if ( !serialConnection )
    {
        throw std::runtime_error{ "Baud rate change only available for serial connections" };
    }

    const auto brIter = std::find( m4BaudRates.cbegin(), m4BaudRates.cend(), baudRate );
    if ( m4BaudRates.cend() == brIter )
    {
        throw ECommException( ExceptionSeverity::Stop, CommunicationError::Unspecified, "Unsupported baud rate" );
    }
    ByteType baudRateIdx = static_cast< ByteType >( m4BaudRates.cend() - brIter );
    LOG_WRITE( LOG_INFO, LOCALIZED( "Setting baud rate to " )
        << ToLocString( connections::BaudRateToString( baudRate ) ) );

    connections::BaudRate::Type prevBaudRate = serialConnection->GetBaudRate();
    bool changedOk = false;
    bool devAcksNewBaudRate = false;
    
    try
    {
        ByteVector pData{ baudRateIdx, 0, 0, 0 };
        Packet response = DoLegacyRequest( nt, Opcode::SetSpeed, pData, 0, RecvFlags::DontThrowOnErrorReply );
        if ( response.funcOpcode == Opcode::SetSpeed )
        {
            /// Поступил запрос от прибора на изменение BaudRate
            devAcksNewBaudRate = true;
            WaitFor( 250 );
            serialConnection->Purge( connections::PurgeFlags::TxRx );
            /// Проверяем, что прибор отвечает на новой скорости
            serialConnection->SetBaudRate( baudRate );
            pData = ByteVector{ static_cast< ByteType >( tv ), 0, 0, 0 };
            response = DoLegacyRequest( nt, Opcode::Handshake, pData, 3, RecvFlags::DontThrowOnErrorReply );
            changedOk = response.funcOpcode == Opcode::Handshake;
            LOG_WRITE( LOG_DEBUG, LOCALIZED( "Set new baud rate: " )
                << ToLocString( connections::BaudRateToString( baudRate ) ) );
        }
    }
    catch ( const ECommException& e )
    {
        if (   e.GetReason() != CommunicationError::Timeout
            && e.GetReason() != CommunicationError::Checksum )
        {
            throw;
        }
        changedOk = false;
    }
    catch ( const std::exception& e )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Unspecified error" ) );
        throw;
    }

    if ( !changedOk )
    {
        LocString message = LOCALIZED( "Error occured" );
        if ( devAcksNewBaudRate )
        {
            message += LOCALIZED( ", restoring previous baud rate..." );
        }
        LOG_WRITE( LOG_WARNING, message );
        serialConnection->SetBaudRate( prevBaudRate );
        if ( devAcksNewBaudRate )
        {
            constexpr uint32_t fallbackTimeout = static_cast< uint32_t >( ALT_SPEED_FALLBACK_TIME * 1.1 );
            WaitFor( fallbackTimeout );
            LOG_WRITE( LOG_INFO, LOCALIZED( "Restored previous baud rate " )
                << ToLocString( connections::BaudRateToString( prevBaudRate ) ) );
        }
    }
    return changedOk;
} // SetBusSpeed


void M4Protocol::CloseCommSessionImpl( ByteType* srcNt, ByteType* dstNt )
{
    (void) srcNt;
    DoLegacyRequest( dstNt, Opcode::SessionClose, ByteVector( 4, 0x0 ), 0, RecvFlags::DontThrowOnErrorReply );
} // CloseCommSessionImpl


void M4Protocol::SerialSpeedFallback()
{
    if ( initialBaudRate_ == connections::BaudRate::NotSupported )
    {
        return;
    }
    std::shared_ptr< connections::SerialConnection > serialConnection = ConnectionAsSerial( connection_ );
    if ( !serialConnection )
    {
        return;
    }

    if ( serialConnection->GetBaudRate() == initialBaudRate_ )
    {
        return;
    }
    serialConnection->SetBaudRate( initialBaudRate_ );
    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Restored default baud rate: " )
        << ToLocString( connections::BaudRateToString( initialBaudRate_ ) ) );
} // SerialSpeedFallback


void M4Protocol::OnRecoverableError()
{
    if ( activeDev_ )
    {
        activeDev_->hasIoError = true;
    }
} // OnRecoverableError

} // namespace M4

} // namespace protocols

} // namespace logika

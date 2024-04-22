/// @file Реализация протокола M4 (основные методы)
/// @copyright HypeRRu 2024
/// @see m4protocol_api.cpp

#include <logika/protocols/m4/m4protocol.h>

#include <logika/log/defines.h>

#include <logika/protocols/comm_exception.h>
#include <logika/protocols/m4/opcodes.h>
#include <logika/protocols/m4/tag_write_data.h>
#include <logika/protocols/m4/archive4m.h>
#include <logika/connections/iconnection.h>
#include <logika/connections/utils/types_converter.h>
#include <logika/connections/serial/serial_connection.h>
#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/meters/logika4/4l/spg741.h>
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
        || !( connection->GetConnectionType() & logika::connections::ConnectionType::Serial ) )
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


/// @brief Конвертация номера ошибки в код
/// @param[in] errCode Номер ошибки
/// @return Код ошибки
logika::protocols::Rc::Type RcFromError( logika::ByteType errCode )
{
    switch ( errCode )
    {
        case 0:
            return logika::protocols::Rc::BadRequestError;
        case 1:
            return logika::protocols::Rc::WriteProtectedError;
        case 2:
            return logika::protocols::Rc::InvalidArgError;
        default:
            return logika::protocols::Rc::Fail;
    }
} // RcFromError

/// @brief Приведение 32 битного целого числа к LE порядку байтов
/// @param[in] number
/// @return Число number в LE порядке байтов
uint32_t UintToLe( uint32_t number )
{
    volatile uint32_t i = 0x01234567;
    // return false for big endian, true for little endian.
    if ( ( *reinterpret_cast< volatile uint8_t* >( &i ) ) == 0x67 )
    {
        return number;
    }
    return ( ( ( number >> 24 ) & 0xFF ) <<  0 )
        +  ( ( ( number >> 16 ) & 0xFF ) <<  8 )
        +  ( ( ( number >>  8 ) & 0xFF ) << 16 )
        +  ( ( ( number >>  0 ) & 0xFF ) << 24 );
} // UintToLe

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
const uint32_t M4Protocol::MAX_PAGE_BLOCK           = 8;
const uint32_t M4Protocol::CHANNEL_NBASE            = 100000;


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


M4Protocol::M4Protocol( const storage::StorageKeeper& sKeeper,
    connections::BaudRate::Type targetBaudRate )
    : Protocol( sKeeper )
    , initialBaudRate_{ connections::BaudRate::NotSupported }
    , suggestedBaudRate_{ targetBaudRate }
    , activeDev_{ nullptr }
    , metadataCache_{}
    , archivePartition_{ M4Protocol::PARTITION_CURRENT }
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


std::shared_ptr< meters::Meter > M4Protocol::GetMeterType( const ByteType* srcNt,
    const ByteType* dstNt, LocString& extraData )
{
    Packet handshakePacket = DoHandshake( dstNt, 0, false );
    if ( handshakePacket.data.size() == 3 )
    {
        extraData = ToLocString( std::to_string( handshakePacket.data.at( 2 ) ) );
    }
    return meters::Logika4::DetermineMeter(
        handshakePacket.data.at( 0 ), handshakePacket.data.at( 1 ), handshakePacket.data.at( 2 ),
        sKeeper_.GetStorage< LocString, meters::Meter >()
    );
} // GetMeterType


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
    const ByteType* zNt, MeterChannel::Type tv )
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
                    sKeeper_.GetStorage< LocString, meters::Meter >()
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


ByteVector M4Protocol::GenerateRawHandshake( const ByteType* dstNt )
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


void M4Protocol::AppendParamNum( ByteVector& buffer, ByteType channel, uint16_t ordinal )
{
    buffer.push_back( static_cast< ByteType >( 0x4A ) );
    buffer.push_back( static_cast< ByteType >( 0x03 ) );

    buffer.push_back( channel );
    buffer.push_back( static_cast< ByteType >( ordinal & 0xFF ) );
    buffer.push_back( static_cast< ByteType >( ( ordinal >> 8 ) & 0xFF ) );
} // AppendParamNum


std::vector< meters::Logika4M::Tag4MRecordType > M4Protocol::ParseM4TagsPacket( const Packet& packet,
    std::vector< bool >& opFlags )
{
    if ( !packet.extended || packet.funcOpcode != Opcode::ReadTags )
    {
        throw std::invalid_argument{ "Invalid packet passed" };
    }
    std::vector< meters::Logika4M::Tag4MRecordType > tags;
    opFlags.clear();

    MeterAddressType tagPos = 0;
    while ( tagPos < packet.data.size() )
    {
        MeterAddressType readLen = 0;
        try
        {
            auto tagData = meters::Logika4M::ParseTag( packet.data, tagPos, readLen );
            if ( tagData.first == meters::TagId4M::Oper )
            {
                /// Флаг оперативности идет вслед за считанным тэгом
                auto& data = tagData.second;
                if ( !data || opFlags.empty() )
                {
                    throw std::logic_error{ "Empty oper tag or tags list is empty" };
                }
                meters::OperParamFlag::Type oper = data->Cast< meters::OperParamFlag::Type >();
                opFlags.back() = ( oper == meters::OperParamFlag::Yes );
                tagPos += readLen;
                continue;
            }
            tagPos += readLen;
            tags.emplace_back( std::move( tagData ) );
            opFlags.push_back( false );
        }
        catch ( const std::exception& )
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Tags parsing failed at 0x" )  << std::hex << tagPos );
            throw;
        }
    }
    return tags;
} // ParseM4TagsPacket


TimeType M4Protocol::RestrictTime( TimeType date )
{
    constexpr TimeType restrictTimestamp = 8993721600000; /// 01-01-2025 00:00:00
    return date < restrictTimestamp ? date : restrictTimestamp;
} // RestrictTime


std::vector< std::shared_ptr< ArchiveRecord > > M4Protocol::ParseArchivePacket4M(
    const Packet& packet, TimeType& nextRecord )
{
    if ( !packet.extended || packet.funcOpcode != Opcode::ReadArchive )
    {
        throw std::invalid_argument{ "Invalid packet" };
    }
    std::vector< std::shared_ptr< ArchiveRecord > > records;
    nextRecord = 0;

    MeterAddressType readLen = 0;
    /// Can be 0x49 (datetime stamp) or 0x04 (FLZ compressed records)
    meters::Logika4M::Tag4MRecordType firstTag = meters::Logika4M::ParseTag( packet.data, 0, readLen );
    ByteVector decompressedData{};
    if ( meters::TagId4M::ByteArray == firstTag.first )
    {
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Compressed data" ) );
        MeterAddressType tailLength = packet.data.size() - readLen;
        ByteVector buffer{};
        if ( firstTag.second )
        {
            firstTag.second->TryCast< ByteVector >( buffer );
        }
        decompressedData = FastLzImpl::Decompress( buffer, 0, buffer.size() );
        /// Копирование оставшихся данных
        for ( size_t i = readLen; i < packet.data.size(); ++i )
        {
            decompressedData.push_back( packet.data.at( i ) );
        }
    }
    else
    {
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Not compressed data" ) );
        decompressedData = packet.data;
    }
    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Data size: " ) << decompressedData.size() << LOCALIZED( " bytes" ) );

    MeterAddressType tp = 0;
    while ( tp < decompressedData.size() )
    {
        /// 0x49 tag (sync archives: shortened datetime, async archives: full datetime)
        readLen = 0;
        meters::Logika4M::Tag4MRecordType tagTime = meters::Logika4M::ParseTag(
            decompressedData, tp, readLen );
        TimeType timeFromTag = 0;
        tagTime.second->TryCast< TimeType >( timeFromTag );
        if ( tagTime.first != meters::TagId4M::Timestamp )
        {
            throw std::runtime_error{ "Wrong tag type" };
        }
        tp += readLen;
        MeterAddressType recordLen = 0;
        MeterAddressType offset = meters::Logika4M::GetTagLength( decompressedData, tp + 1, recordLen );
        if ( 0 == recordLen )
        {
            nextRecord = timeFromTag;
            break;
        }

        std::shared_ptr< ArchiveRecord > record = std::make_shared< ArchiveRecord >();
        record->timeSinceStart = timeFromTag;
        if ( decompressedData.at( tp ) == static_cast< ByteType >( 0x30 ) )
        {
            /// Синхронный архив
            tp += 1 + offset;
            MeterAddressType start = tp;
            std::vector< meters::Logika4M::Tag4MRecordType > archiveContent{};
            while ( ( tp - start ) < recordLen )
            {
                archiveContent.emplace_back(
                    meters::Logika4M::ParseTag( decompressedData, tp, readLen )
                );
                tp += readLen;
            }
            LocString time = LOCALIZED( "" );
            LocString date = LOCALIZED( "" );
            if ( archiveContent.size() < 2
                || !archiveContent.at( 0 ).second->TryCast< LocString >( time )
                || !archiveContent.at( 1 ).second->TryCast< LocString >( date )
                || time.length() != 8
                || date.length() != 8 )
            {
                record->fullTime = meters::Logika4::CombineDateTime( date, time );
                archiveContent.erase( archiveContent.begin(), archiveContent.begin() + 2 );
            }
            else
            {
                record->fullTime = 0;
            }
            record->values.clear();
            record->values.reserve( archiveContent.size() );
            for ( const auto& value: archiveContent )
            {
                record->values.emplace_back( value.second );
            }
        }
        else
        {
            /// Асинхронный архив
            record->fullTime = record->timeSinceStart;
            record->values.clear();
            record->values.emplace_back( meters::Logika4M::ParseTag(
                decompressedData, tp, readLen ).second );
            tp += readLen;
        }
        records.emplace_back( record );
    }

    if ( nextRecord != 0 && records.size() > 0 && nextRecord <= records.back()->fullTime )
    {
        throw ECommException( ExceptionSeverity::Stop, CommunicationError::Unspecified, "Archive dates cycling detected" );
    }

    return records;
} // ParseArchivePacket4M


Packet M4Protocol::RecvPacket( const ByteType* expectedNt, Opcode::Type* expectedOpcode, const ByteType* expectedId,
    uint32_t expectedDataLength, RecvFlags::Type flags )
{
    ByteVector buffer;
    ByteVector check;
    uint32_t readed     = 0;
    uint32_t payloadLen = 0;

    Packet packet{};
    if ( !connection_ || !connection_->IsConnected() )
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
                readed = connection_->Read( buffer, 0, 1 );
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
            readed = connection_->Read( buffer, 1, 2 );
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
            if ( expectedNt && packet.nt != *expectedNt )
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
                readed = connection_->Read( buffer, 3, 5 );
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
                if ( expectedOpcode && packet.funcOpcode != *expectedOpcode
                    && static_cast< Opcode::Type >( packet.funcOpcode ) != Opcode::Error )
                {
                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Unmatched opcode: " )
                        << std::hex << static_cast< uint8_t >( packet.funcOpcode ) << LOCALIZED( " (packet) vs " )
                        << std::hex << static_cast< uint8_t >( *expectedOpcode ) << LOCALIZED( " (expected)" ) );
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
                LOG_WRITE( LOG_DEBUG, LOCALIZED( "Payload length: " ) << payloadLen );
            }
            /// data + CSUM + frame end
            readed = connection_->Read( packet.data, 0, payloadLen );
            if ( readed < payloadLen )
            {
                OnRecoverableError();
                /// @bug Сделать повторное чтение?
                throw ECommException( ExceptionSeverity::Error,
                    CommunicationError::SystemError, "General read error" );
            }
            /// legacy: checksum8 + frame end, M4: CRC16
            readed = connection_->Read( check, 0, 2 );
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
                packet.checkSum = check.at( 1 ) & 0xFF;
                packet.checkSum |= check.at( 0 ) << 8;
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
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Extended packet" ) );
        /// Расчет CRC16
        calculatedChecksum = Protocol::Crc16( calculatedChecksum, buffer, 1, 7 );
        calculatedChecksum = Protocol::Crc16( calculatedChecksum, packet.data, 0, packet.data.size() );
    }
    else
    {
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Legacy packet" ) );
        /// Расчет Checksum8
        calculatedChecksum = 0x1600; /// END_OF_FRAME << 8
        /// header + data
        calculatedChecksum |= ~static_cast< ByteType >(
            ~meters::Logika4::CheckSum8( buffer, 1, 2 ) +
            ~meters::Logika4::CheckSum8( packet.data, 0, packet.data.size() )
        );
    }
    if ( packet.checkSum != calculatedChecksum )
    {
        /// Несовпадение контрольной суммы
        RegisterEvent( Rc::RxCrcError );
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Checksum not matched: " ) <<
            std::hex << packet.checkSum << LOCALIZED( "(packet) vs " ) <<
            std::hex << calculatedChecksum << LOCALIZED( "(calculated)" ) );
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
                "Error code: " + std::to_string( errorCode ),
                LOCALIZED( "Device returned error: " )
                    + ToLocString( std::to_string( errorCode ) ) );
        }
    }

    return packet;
} // RecvPacket


void M4Protocol::SendLegacyPacket( const ByteType* nt, Opcode::Type func, const ByteVector& data )
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


void M4Protocol::SendExtendedPacket( const ByteType* nt, ByteType packetId, Opcode::Type opcode, const ByteVector& data )
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


Packet M4Protocol::DoLegacyRequest( const ByteType* nt, Opcode::Type requestOpcode, const ByteVector& data,
    uint32_t expectedDataLength, RecvFlags::Type flags )
{
    SendLegacyPacket( nt, requestOpcode, data );
    return RecvPacket( nt, &requestOpcode, nullptr, expectedDataLength, flags );
} // DoLegacyRequest


Packet M4Protocol::DoM4Request( const ByteType* nt, Opcode::Type requestOpcode, const ByteVector& data,
    const ByteType* packetId, RecvFlags::Type flags )
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


Packet M4Protocol::DoHandshake( const ByteType* nt, ByteType channel, bool slowWake )
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


bool M4Protocol::SetBusSpeed( std::shared_ptr< meters::Logika4 > meter, const ByteType* nt,
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


Rc::Type M4Protocol::WriteParameterL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
    ByteType channel, uint32_t paramNum, const LocString& value, bool operFlag )
{
    const auto devStorage = sKeeper_.GetStorage< LocString, meters::Meter >();
    std::shared_ptr< meters::Meter >  baseSpg741    = nullptr;
    std::shared_ptr< meters::Spg741 > spg741Meter   = nullptr;
    if ( devStorage && devStorage->GetItem( LOCALIZED( "SPG741" ), baseSpg741 ) )
    {
        spg741Meter = std::dynamic_pointer_cast< meters::Spg741 >( baseSpg741 );
    }
    if ( spg741Meter && meter == spg741Meter && paramNum >= 200 && paramNum <= 300 )
    {
        /// Транслируемые через СП параметры СПГ741
        const auto tagsVault = spg741Meter->GetTagsVault();
        if ( !tagsVault )
        {
            return Rc::Success;
        }
        const auto tags = tagsVault->All();
        const auto tagDefIter = std::find_if( tags.cbegin(), tags.cend(), [ paramNum ](
            const std::shared_ptr< meters::DataTagDef >& tag ) {
            return tag && tag->GetOrdinal() == paramNum;
        } );
        if ( tags.cend() == tagDefIter )
        {
            return Rc::Success;
        }
        ByteType sp = GetSpg741Sp( nt );
        int32_t mappedOrdinal = meters::Spg741::GetMappedDbParamOrdinal( ( *tagDefIter )->GetKey(), sp );
        if ( -1 == mappedOrdinal )
        {
            /// Параметр не существует при текущем СП
            return Rc::Success;
        }
        paramNum = static_cast< uint32_t >( mappedOrdinal );
    }
    SelectDeviceAndChannel( meter, nt, static_cast< MeterChannel::Type >( channel ) );
    if ( channel == 1 || channel == 2 )
    {
        /// При записи указывается относительный номер параметра по каналу
        constexpr uint32_t shift = 50;
        paramNum = paramNum > shift ? paramNum - shift : 0;
    }
    ByteVector requestData{
          static_cast< ByteType >( paramNum & 0xFF )
        , static_cast< ByteType >( ( paramNum >> 8 ) & 0xFF )
        , 0, 0
    };
    /// Проверка записи
    Packet packet = DoLegacyRequest( nt, Opcode::WriteParam, requestData, 0, RecvFlags::DontThrowOnErrorReply );
    if ( packet.funcOpcode == Opcode::Error )
    {
        if ( packet.data.empty() )
        {
            return Rc::Fail;
        }
        return RcFromError( packet.data.at( 0 ) );
    }
    /// Запись параметра
    requestData = ByteVector( 64, 0x0 );
    for ( size_t i = 0; i < requestData.size(); ++i )
    {
        if ( i < value.length() )
        {
            requestData[ i ] = static_cast< ByteType >( value.at( i ) );
        }
        else
        {
            requestData[ i ] = static_cast< ByteType >( 0x20 );
        }
    }
    if ( operFlag )
    {
        requestData.back() = static_cast< ByteType >( '*' );
    }
    
    packet = DoLegacyRequest( nt, Opcode::WriteParam, requestData, 0, RecvFlags::DontThrowOnErrorReply );
    if ( packet.funcOpcode == Opcode::Error )
    {
        if ( packet.data.empty() )
        {
            return Rc::Fail;
        }
        return RcFromError( packet.data.at( 0 ) );
    }
    return Rc::Success;
} // WriteParameterL4


ByteVector M4Protocol::ReadRamL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
    MeterAddressType startAddr, MeterAddressType numBytes )
{
    if ( numBytes > M4Protocol::MAX_RAM_REQUEST )
    {
        throw std::invalid_argument{ "Too much data requested from RAM" };
    }

    SelectDeviceAndChannel( meter, nt );
    const ByteVector requestData{
          static_cast< ByteType >( startAddr & 0xFF )
        , static_cast< ByteType >( ( startAddr >> 8 ) & 0xFF )
        , static_cast< ByteType >( numBytes ), 0
    };
    Packet packet = DoLegacyRequest( nt, Opcode::ReadRam, requestData, numBytes );
    return packet.data;
} // ReadRamL4


ByteVector M4Protocol::ReadFlashPagesL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
    uint32_t startPage, uint32_t pageCount )
{
    if ( pageCount == 0 )
    {
        throw std::invalid_argument{ "Attempt to read zero pages of flash" };
    }

    const MeterAddressType pageSize = meters::Logika4L::FLASH_PAGE_SIZE;

    SelectDeviceAndChannel( meter, nt );
    ByteVector cmdBuffer( 4, 0x0 );
    ByteVector retbuf{};
    retbuf.reserve( pageCount * pageSize );

    const size_t pagesToRead = ( pageCount + MAX_PAGE_BLOCK - 1 ) / MAX_PAGE_BLOCK;
    for ( size_t page = 0; page < pagesToRead; ++page )
    {
        uint32_t requestPagesCount = pageCount - page * MAX_PAGE_BLOCK;
        if ( requestPagesCount > MAX_PAGE_BLOCK )
        {
            requestPagesCount = MAX_PAGE_BLOCK;
        }
        uint32_t blockStart = startPage + page * MAX_PAGE_BLOCK;
        const ByteVector requestData{
              static_cast< ByteType >( blockStart & 0xFF )
            , static_cast< ByteType >( ( blockStart >> 8 ) & 0xFF )
            , static_cast< ByteType >( requestPagesCount ), 0
        };
        SendLegacyPacket( nt, Opcode::ReadFlash, requestData );
        /// Чтение страниц флэш-памяти
        for ( uint32_t i = 0; i < requestPagesCount; ++i )
        {
            Packet packet;
            try
            {
                Opcode::Type opcode = Opcode::ReadFlash;
                packet = RecvPacket( nt, &opcode, nullptr, pageSize );
            }
            catch ( const std::exception& )
            {
                if ( pageCount > 1 )
                {
                    /// При ошибке в многостраничных запросах флеша - пересинхронизируем поток чтения,
                    /// чтобы не словить опоздавшую страницу с другим адресом
                    OnRecoverableError();
                }
                throw;
            }
            if ( packet.funcOpcode != Opcode::ReadFlash || packet.data.size() != pageSize )
            {
                throw ECommException( ExceptionSeverity::Error, CommunicationError::Unspecified,
                    "Invalid packet read, opcode is " + std::to_string( packet.funcOpcode ) );
            }
            for ( size_t j = 0; j < packet.data.size(); ++j )
            {
                retbuf.push_back( packet.data.at( j ) );
            }
        }
    }
    return retbuf;
} // ReadFlashPagesL4


ByteVector M4Protocol::ReadFlashBytesL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
    MeterAddressType startAddr, MeterAddressType length )
{
    if ( length == 0 )
    {
        throw std::invalid_argument{ "Attempt to read zero bytes of flash" };
    }

    const MeterAddressType pageSize = meters::Logika4L::FLASH_PAGE_SIZE;
    const uint32_t startPage    = startAddr / pageSize;
    const uint32_t endPage      = ( startAddr + length - 1 ) / pageSize;
    const uint32_t pageCount    = endPage - startPage + 1;
    
    ByteVector memory = ReadFlashPagesL4( meter, nt, startPage, pageCount );
    memory.erase( memory.begin(), memory.begin() + ( startAddr % pageSize ) );
    memory.resize( length );
    return memory;
} // ReadFlashBytesL4


std::vector< meters::Logika4M::Tag4MRecordType > M4Protocol::ReadTags4M( std::shared_ptr< meters::Logika4M > meter,
    const ByteType* nt, const std::vector< int32_t >& channels,
    const std::vector< int32_t >& ordinals, std::vector< bool >& opFlags )
{
    SelectDeviceAndChannel( meter, nt );
    if ( channels.empty() || ordinals.empty() || channels.size() != ordinals.size() )
    {
        throw std::invalid_argument{ "Invalid parameters for ReadTags4M" };
    }
    ByteVector requestData;
    for ( size_t i = 0; i < ordinals.size(); ++i )
    {
        ByteType channel = static_cast< ByteType >(
            ordinals.at( i ) < M4Protocol::CHANNEL_NBASE
            ? channels.at( i )
            : channels.at( i ) / M4Protocol::CHANNEL_NBASE
        );
        uint16_t ordinal = static_cast< uint16_t >( ordinals.at( i ) % M4Protocol::CHANNEL_NBASE );
        AppendParamNum( requestData, channel, ordinal );
    }

    Packet packet = DoM4Request( nt, Opcode::ReadTags, requestData );
    std::vector< meters::Logika4M::Tag4MRecordType > tagsData = M4Protocol::ParseM4TagsPacket( packet, opFlags );
    if (   meter->GetCaption() == LOCALIZED( "SPG742" )
        || meter->GetCaption() == LOCALIZED( "SPT941_20" ) )
    {
        /// Серийный номер СПxx4x (M4) - отрезаем старший байт
        for ( size_t i = 0; i < ordinals.size(); ++i )
        {
            if ( ordinals.at( i ) == 8256 )
            {
                auto& data = tagsData.at( i ).second;
                if ( !data )
                {
                    continue;
                }
                uint32_t uintData;
                if ( data->TryCast< uint32_t >( uintData ) )
                {
                    tagsData[ i ] = std::make_pair(
                        tagsData.at( i ).first,
                        std::make_shared< logika::Any >( uintData & 0x00FFFFFF )
                    );
                }
                else
                {
                    continue;
                }
            }
        }
    }

    return tagsData;
} // ReadTags4M


std::vector< Rc::Type > M4Protocol::WriteParams4M( std::shared_ptr< meters::Logika4M > meter, const ByteType* nt,
    const std::vector< TagWriteData >& data )
{
    SelectDeviceAndChannel( meter, nt );

    ByteVector requestData;
    for ( const TagWriteData& tagData: data )
    {
        std::shared_ptr< logika::Any > value = tagData.value;
        bool found = false;
        LocString sValue;
        if ( !value || value->Empty()
            || ( value->TryCast< LocString >( sValue ) && sValue.empty() ) )
        {
            requestData.push_back( static_cast< ByteType >( meters::TagId4M::Empty ) ); /// null
            requestData.push_back( static_cast< ByteType >( 0x00 ) );
            found = true;
        }
        if ( !found && value->TryCast< LocString >( sValue ) )
        {
            requestData.push_back( static_cast< ByteType >( meters::TagId4M::Ia5String ) ); /// ASCII string
            requestData.push_back( static_cast< ByteType >( sValue.length() ) );
            for ( size_t i = 0; i < sValue.length(); ++i )
            {
                requestData.push_back( static_cast< ByteType >( sValue.at( i ) ) );
            }
            found = true;
        }
        uint32_t uValue;
        if ( !found && value->TryCast< uint32_t >( uValue ) )
        {
            requestData.push_back( static_cast< ByteType >( meters::TagId4M::I32LE ) ); /// unsigned int
            requestData.push_back( static_cast< ByteType >( 0x04 ) ); /// length
            uValue = UintToLe( uValue );
            for ( size_t i = 0; i < 4; ++i )
            {
                /// @todo Проверить правильность заполнения
                requestData.push_back( static_cast< ByteType >( ( uValue >> ( 8 * ( 3 - i ) ) ) & 0xFF ) );
            }
            found = true;
        }
        ByteVector baValue;
        if ( !found && value->TryCast< ByteVector >( baValue ) )
        {
            requestData.push_back( static_cast< ByteType >( meters::TagId4M::ByteArray ) ); /// octet string
            if ( baValue.size() < 0x80 )
            {
                requestData.push_back( static_cast< ByteType >( baValue.size() ) ); // single-byte length
            }
            else if ( baValue.size() < 0x10000 )
            {
                requestData.push_back( static_cast< ByteType >( 0x82 ) ); /// two-byte length
                /// @todo Проверить правильность заполнения
                requestData.push_back( static_cast< ByteType >( ( baValue.size() >> 8 ) & 0xFF ) );
                requestData.push_back( static_cast< ByteType >( baValue.size() & 0xFF ) );
            }
            else
            {
                throw std::invalid_argument{ "Octet string is too large" };
            }
            for ( ByteType b: baValue )
            {
                requestData.push_back( b );
            }
            found = true;
        }
        if ( !found )
        {
            throw std::invalid_argument{ "Unsupported data tag value" };
        }

        if ( tagData.oper )
        {
            requestData.push_back( static_cast< ByteType >( meters::TagId4M::Oper ) ); /// oper tag
            requestData.push_back( static_cast< ByteType >( 0x01 ) );
            requestData.push_back( static_cast< ByteType >( tagData.oper ) );
        }
    }

    Packet packet = DoM4Request( nt, Opcode::WriteTags, requestData );
    std::vector< Rc::Type > errors;

    MeterAddressType tagPos = 0x0;
    for ( size_t i = 0; i < data.size(); ++i )
    {
        ByteType tagId = packet.data.at( tagPos );
        ByteType tagLength = packet.data.at( tagPos + 1 );
        if ( tagId == static_cast< ByteType >( meters::TagId4M::Ack ) ) /// ACK
        {
            tagLength = 0;
            errors[ i ] = Rc::Success;
        }
        else if ( tagId == static_cast< ByteType >( meters::TagId4M::Error ) ) /// ERR
        {
            errors[ i ] = RcFromError( packet.data.at( tagPos + 2 ) );
        }
        else
        {
            errors[ i ] = Rc::Fail; /// Unkown tag code
        }
        tagPos += 2 + tagLength;
    }

    return errors;
} // WriteParams4M


Packet M4Protocol::ReadArchive4M( std::shared_ptr< meters::Logika4M > meter, const ByteType* nt, const ByteType* packetId,
    uint16_t partition, const ByteType channel, ArchiveId4M::Type archiveKind, TimeType from, TimeType to,
    uint32_t nValues, std::vector< std::shared_ptr< ArchiveRecord > >& result, TimeType& nextRecord )
{
    SelectDeviceAndChannel( meter, nt );
    Packet packet;
    if ( !meter )
    {
        return packet;
    }

    from    = RestrictTime( from );
    to      = RestrictTime( to );
    if ( 0 != to && from > to )
    {
        LOG_WRITE( LOG_WARNING, LOCALIZED( "Протокол M4 не поддерживает чтение в обратном порядке, запрос[" )
            << from << LOCALIZED( " .. " ) << to << LOCALIZED( "]" ) );
        result.clear();
        nextRecord = 0;
        return packet;
    }

    ByteVector bytes;
    bytes.push_back( static_cast< ByteType >( 0x04 ) );
    bytes.push_back( static_cast< ByteType >( 0x05 ) );
    bytes.push_back( static_cast< ByteType >( partition & 0xFF ) );             /// Раздел(отсчет) L 
    bytes.push_back( static_cast< ByteType >( ( partition >> 8 ) & 0xFF ) );    /// Раздел(отсчет) H 
    bytes.push_back( channel );

    ByteType compressionFlags = static_cast< ByteType >( archiveKind );
    if ( meter->GetSupportFlz() )
    {
        compressionFlags |= static_cast< ByteType >( CompressionType::FlzLimitedLength );
    }

    bytes.push_back( compressionFlags ); /// Опции сжатия и код архива
    if ( nValues > 0xFF )
    {
        nValues = 0xFF;
    }
    bytes.push_back( static_cast< ByteType >( nValues ) ); /// Максимальное количество записей в ответе

    /// @note Обход ошибки в СПТ943r3, из за которой может
    /// не выдаваться запись при указании даты в запросе месячного архива
    bool need943MHack = meter->GetCaption() == LOCALIZED( "SPT943rev3" ) && archiveKind == ArchiveId4M::Mon;
    AppendDateTag( bytes, from, need943MHack );
    if ( 0 != to )
    {
        AppendDateTag( bytes, to, need943MHack );
    }
    LOG_WRITE( LOG_DEBUG, LOCALIZED( "M4 archive request: " )
        << LOCALIZED( "part=" ) << partition
        << LOCALIZED( ", archive=" ) << static_cast< uint32_t >( archiveKind )
        << LOCALIZED( ", channel=" ) << static_cast< uint32_t >( channel )
        << LOCALIZED( ", from=" ) << from
        << LOCALIZED( ", to=" ) << ToLocString( 0 != to ? std::to_string( to ) : "[null]" )
        << LOCALIZED( ", maxCnt=" ) << nValues );
    packet = DoM4Request( nt, Opcode::ReadArchive, bytes, packetId );
    result = M4Protocol::ParseArchivePacket4M( packet, nextRecord );
    LOG_WRITE( LOG_DEBUG, LOCALIZED( "M4 answer: " )
        << result.size() << LOCALIZED( " records" )
        << LOCALIZED( ", next date pointer " ) << nextRecord );

    return packet;
} // ReadArchive4M


void M4Protocol::CloseCommSessionImpl( const ByteType* srcNt, const ByteType* dstNt )
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


ByteType M4Protocol::GetSpg741Sp( const ByteType* nt )
{
    const auto mtrStorage = sKeeper_.GetStorage< LocString, meters::Meter >();
    if ( !mtrStorage )
    {
        throw std::runtime_error{ "Can't get meter storage" };
    }
    std::shared_ptr< meters::Meter > mtr = nullptr;
    mtrStorage->GetItem( LOCALIZED( "SPG741" ), mtr );
    std::shared_ptr< meters::Spg741 > spg741 = std::dynamic_pointer_cast< meters::Spg741 >( mtr );
    if ( !spg741 )
    {
        throw std::runtime_error{ "Can't get SPG741 meter from storage" };
    }
    std::shared_ptr< MeterCache > mtrInstance = GetMeterInstance( spg741, nt );
    if ( static_cast< ByteType >( -1 ) == mtrInstance->sp_ )
    {
        constexpr MeterAddressType SP_741_ADDR = 0x200;
        GetFlashPagesToCache( spg741, nt, SP_741_ADDR / meters::Logika4L::FLASH_PAGE_SIZE, 1, mtrInstance );
        bool oper = false;
        LocString spVal = meters::Logika4L::GetDbEntryValue( mtrInstance->flash_, SP_741_ADDR, oper );
        try
        {
            mtrInstance->sp_ = static_cast< ByteType >( std::stoull( spVal ) );
        }
        catch ( const std::exception& )
        {
            mtrInstance->sp_ = 0;
        }
    }
    return static_cast< ByteType >( -1 ) != mtrInstance->sp_ ? mtrInstance->sp_ : 0;
} // GetSpg741Sp


void M4Protocol::AppendDateTag( ByteVector& buffer, TimeType date, bool useMonthYearOnly )
{
    uint16_t millis = 0;
    struct tm timeStruct = GetTimeStruct( date, &millis );

    buffer.push_back( static_cast< ByteType >( meters::TagId4M::Timestamp ) ); // datetime tag
    buffer.push_back( static_cast< ByteType >( useMonthYearOnly ? 2 : 8 ) );
    buffer.push_back( static_cast< ByteType >( timeStruct.tm_year - 100 ) );
    buffer.push_back( static_cast< ByteType >( timeStruct.tm_mon + 1 ) );
    if ( !useMonthYearOnly )
    {
        buffer.push_back( static_cast< ByteType >( timeStruct.tm_mday ) );
        buffer.push_back( static_cast< ByteType >( timeStruct.tm_hour ) );
        buffer.push_back( static_cast< ByteType >( timeStruct.tm_min ) );
        buffer.push_back( static_cast< ByteType >( timeStruct.tm_sec ) );
        buffer.push_back( static_cast< ByteType >( millis & 0xFF ) );
        buffer.push_back( static_cast< ByteType >( ( millis >> 8 ) & 0xFF ) );
    }
} // AppendDateTag

} // namespace M4

} // namespace protocols

} // namespace logika

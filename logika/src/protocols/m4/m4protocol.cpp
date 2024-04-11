/// @file Реализация протокола M4
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/m4protocol.h>

#include <logika/log/defines.h>

#include <logika/connections/utils/types_converter.h>
#include <logika/connections/serial/serial_connection.h>
#include <logika/common/misc.h>


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


M4Protocol::BusActiveState::BusActiveState( std::shared_ptr< meters::Meter > mtr,
    ByteType ntByte, MeterChannel::Type chan )
    : meter{ mtr }
    , nt{ ntByte }
    , tv{ chan }
    , lastIo{ 0 }
    , hasIoError{ false }
{
    if ( !meter )
    {
        throw std::runtime_error{ "Invalid meter passed" };
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
    constexpr TimeType slowWakeTimeout = 20; ///< 20 мс между отправками

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


void M4Protocol::CloseCommSessionImpl( ByteType srcNt, ByteType dstNt )
{
    /// @todo реализовать
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

} // namespace M4

} // namespace protocols

} // namespace logika

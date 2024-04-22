/// @file Реализация базового класса для работы с соединением
/// @copyright HypeRRu 2024

#include <logika/connections/connection.h>
#include <logika/connections/utils/types_converter.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

namespace logika
{

namespace connections
{

Connection::Connection( const std::string& address, TimeType readTimeout )
    : address_{ address }
    , locAddress_{ ToLocString( address_ ) }
    , readTimeout_{ readTimeout }
    , type_{ ConnectionType::Offline }
    , state_{ ConnectionState::NotConnected }
    , txBytesCount_{ 0 }
    , rxBytesCount_{ 0 }
    , lastRxTime_{ 0 }
    , onAfterConnect_{}
    , onBeforeDisonnect_{}
{} // Connection


Connection::~Connection()
{
    Close();
} // ~Connection


bool Connection::Open()
{
    if ( ConnectionState::Connected == state_ )
    {
        LOG_WRITE( LOG_WARNING, LOCALIZED( "Found active connection to " )
            << locAddress_ << LOCALIZED( ". Closing" ) );
        Close();
    }
    LOG_WRITE( LOG_INFO, LOCALIZED( "Connecting to " ) << locAddress_
        << LOCALIZED( " (" ) << ToLocString( ConnectionTypeToString( type_ ) ) << LOCALIZED( ")" ) );
    state_ = ConnectionState::Connecting;
    if ( OpenImpl() )
    {
        LOG_WRITE( LOG_INFO, LOCALIZED( "Successfully connected to " ) << locAddress_ );
        state_ = ConnectionState::Connected;
        if ( onAfterConnect_ )
        {
            onAfterConnect_();
        }
        return true;
    }
    else
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Failed to connect to " ) << locAddress_ );
        state_ = ConnectionState::NotConnected;
        return false;
    }
} // Open


void Connection::Close()
{
    if (   ConnectionState::Connected  == state_
        || ConnectionState::Connecting == state_ )
    {
        LOG_WRITE( LOG_INFO, LOCALIZED( "Disconnecting from " ) << locAddress_ );
        state_ = ConnectionState::Disconnecting;
        if ( onBeforeDisonnect_ )
        {
            onBeforeDisonnect_();
        }
        CloseImpl();
        state_ = ConnectionState::NotConnected;
    }
} // Close


const std::string& Connection::GetAddress() const
{
    return address_;
} // GetAddress


TimeType Connection::GetReadTimeout() const
{
    return readTimeout_;
} // GetReadTimeout


void Connection::SetReadTimeout( TimeType timeout )
{
    readTimeout_ = timeout;
} // SetReadTimeout


ConnectionType::Type Connection::GetConnectionType() const
{
    return type_;
} // GetConnectionType


ConnectionState Connection::GetConnectionState() const
{
    return state_;
} // GetConnectionState


bool Connection::IsConnected() const
{
    return state_ == ConnectionState::Connected;
} // IsConnected


uint32_t Connection::GetTxBytesCount() const
{
    return txBytesCount_;
} // GetTxBytesCount


uint32_t Connection::GetRxBytesCount() const
{
    return rxBytesCount_;
} // GetRxBytesCount


TimeType Connection::GetLastRxTime() const
{
    return lastRxTime_;
} // GetLastRxTime


void Connection::Purge( PurgeFlags::Type flags )
{
    if ( !IsConnected() )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Purge " ) << ToLocString( PurgeFlagsToString( flags ) )
            << LOCALIZED( " failed: Not connected" ) );
        return;
    }
    LOG_WRITE( LOG_INFO, LOCALIZED( "Purge " ) << ToLocString( PurgeFlagsToString( flags ) ) );
    PurgeImpl( flags );
    lastRxTime_ = GetCurrentTimestamp();
} // Purge


void Connection::SetOnAfterConnect( const std::function< void() >& hook )
{
    LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Set OnAfterConnect hook" ) );
    onAfterConnect_ = hook;
} // SetOnAfterConnect


void Connection::SetOnBeforeDisonnect( const std::function< void() >& hook )
{
    LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Set OnBeforeDisconnect hook" ) );
    onBeforeDisonnect_ = hook;
} // SetOnBeforeDisonnect


void Connection::ResetStatistics()
{
    LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Reset statistics" ) );
    txBytesCount_ = 0;
    rxBytesCount_ = 0;
} // ResetStatistics


uint32_t Connection::Read( ByteVector& buffer, uint32_t start, uint32_t needed )
{
    uint32_t readed = 0;
    Rc::Type rc = Rc::Success;

    if ( buffer.size() < start + needed )
    {
        LOG_WRITE( LOG_DEBUG, LOCALIZED( "Resizing buffer. Old size: " ) << buffer.size()
            << LOCALIZED( ". New size: " ) << ( start + needed ) );
        buffer.resize( start + needed );
    }

    while ( readed < needed )
    {
        if ( !IsConnected() )
        {
            LOG_WRITE( LOG_INFO, LOCALIZED( "Connection not established. Readed " )
                << readed << LOCALIZED( " bytes" ) );
            break;
        }
        const uint32_t readCurrent = ReadImpl( buffer, start + readed, needed - readed, &rc );
        if ( 0 == readCurrent )
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Read error. Readed " ) << readed << LOCALIZED( " bytes. Rc: " ) << rc );
            break;
        }
        readed += readCurrent;
        lastRxTime_ = GetCurrentTimestamp();
    }

    rxBytesCount_ += readed;
    return readed;
} // Read


uint32_t Connection::Write( const ByteVector& buffer )
{
    uint32_t written = 0;
    Rc::Type rc = Rc::Success;

    while ( written < buffer.size() )
    {
        if ( !IsConnected() )
        {
            LOG_WRITE( LOG_INFO, LOCALIZED( "Connection not established. Written " )
                << written << LOCALIZED( " bytes" ) );
            break;
        }
        const uint32_t writeCurrent = WriteImpl( buffer, written, &rc );
        if ( 0 == writeCurrent )
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Write error. Written " )
                << written << LOCALIZED( " bytes. Rc: " ) << rc );
            break;
        }
        written += writeCurrent;
    }

    txBytesCount_ += written;
    return written;
} // Write


bool Connection::OpenImpl()
{
    return false;
} // OpenImpl


void Connection::CloseImpl()
{
} // CloseImpl


void Connection::PurgeImpl( PurgeFlags::Type flags )
{
    ( void ) flags;
} // PurgeImpl


uint32_t Connection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed, Rc::Type* rc )
{
    ( void ) buffer;
    ( void ) start;
    ( void ) needed;
    if ( rc )
    {
        *rc = Rc::Success;
    }
    return 0;
} // ReadImpl


uint32_t Connection::WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    ( void ) buffer;
    ( void ) start;
    if ( rc )
    {
        *rc = Rc::Success;
    }
    return 0;
} // WriteImpl

} // namespace connections

} // namespace logika

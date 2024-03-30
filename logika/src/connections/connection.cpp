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

/// @todo Заменить ToLocString( address_ ) на locAddress_

Connection::Connection( const std::string& address, uint32_t readTimeout )
    : address_{ address }
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
        LOG_WRITE( LOG_WARNING, L"Found active connection to " << ToLocString( address_ ) << L". Closing" );
        Close();
    }
    LOG_WRITE( LOG_INFO, L"Connecting to " << ToLocString( address_ )
        << L" (" << ToLocString( ConnectionTypeToString( type_ ) ) << L")" );
    state_ = ConnectionState::Connecting;
    if ( OpenImpl() )
    {
        LOG_WRITE( LOG_INFO, L"Successfully connected to " << ToLocString( address_ ) );
        state_ = ConnectionState::Connected;
        if ( onAfterConnect_ )
        {
            onAfterConnect_();
        }
        return true;
    }
    else
    {
        LOG_WRITE( LOG_ERROR, L"Failed to connect to " << ToLocString( address_ ) );
        state_ = ConnectionState::NotConnected;
        return false;
    }
} // Open


void Connection::Close()
{
    if (   ConnectionState::Connected  == state_
        || ConnectionState::Connecting == state_ )
    {
        LOG_WRITE( LOG_INFO, L"Disconnecting from " << ToLocString( address_ ) );
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


uint32_t Connection::GetReadTimeout() const
{
    return readTimeout_;
} // GetReadTimeout


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
        LOG_WRITE( LOG_ERROR, L"Purge " << ToLocString( PurgeFlagsToString( flags ) ) << L" failed: Not connected" );
        return;
    }
    LOG_WRITE( LOG_INFO, L"Purge " << ToLocString( PurgeFlagsToString( flags ) ) );
    PurgeImpl( flags );
} // Purge


void Connection::SetOnAfterConnect( const std::function< void() >& hook )
{
    LOG_WRITE_MSG( LOG_INFO, L"Set OnAfterConnect hook" );
    onAfterConnect_ = hook;
} // SetOnAfterConnect


void Connection::SetOnBeforeDisonnect( const std::function< void() >& hook )
{
    LOG_WRITE_MSG( LOG_INFO, L"Set OnBeforeDisconnect hook" );
    onBeforeDisonnect_ = hook;
} // SetOnBeforeDisonnect


void Connection::ResetStatistics()
{
    LOG_WRITE_MSG( LOG_INFO, L"Reset statistics" );
    txBytesCount_ = 0;
    rxBytesCount_ = 0;
} // ResetStatistics


uint32_t Connection::Read( ByteVector& buffer, uint32_t needed )
{
    uint32_t readed = 0;
    Rc::Type rc = Rc::Success;

    if ( buffer.size() < needed )
    {
        LOG_WRITE( LOG_DEBUG, L"Resizing buffer. New size: " << needed );
        buffer.resize( needed );
    }

    while ( readed < needed )
    {
        if ( !IsConnected() )
        {
            LOG_WRITE( LOG_INFO, L"Connection not established. Readed " << readed << L" bytes" );
            break;
        }
        const uint32_t readCurrent = ReadImpl( buffer, readed, needed - readed, &rc );
        if ( 0 == readCurrent )
        {
            LOG_WRITE( LOG_ERROR, L"Read error. Readed " << readed << L" bytes. Rc: " << rc );
            break;
        }
        readed += readCurrent;
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
            LOG_WRITE( LOG_INFO, L"Connection not established. Written " << written << L" bytes" );
            break;
        }
        const uint32_t writeCurrent = WriteImpl( buffer, written, &rc );
        if ( 0 == writeCurrent )
        {
            LOG_WRITE( LOG_ERROR, L"Write error. Written " << written << L" bytes. Rc: " << rc );
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

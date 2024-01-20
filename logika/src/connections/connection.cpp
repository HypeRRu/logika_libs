/// @file Реализация базового класса для работы с соединением
/// @copyright HypeRRu 2024

#include <logika/connections/connection.h>
#include <logika/connections/utils/types_converter.h>

#include <logika/log/defines.h>

#include <iostream>

namespace logika
{

namespace connections
{

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
        LOG_WRITE( LOG_WARNING, "Found active connection to " << address_ << ". Closing" );
        Close();
    }
    LOG_WRITE( LOG_INFO, "Connecting to " << address_ << " (" << ConnectionTypeToString( type_ ) << ")" );
    state_ = ConnectionState::Connecting;
    if ( OpenImpl() )
    {
        LOG_WRITE( LOG_INFO, "Successfully connected to " << address_ );
        state_ = ConnectionState::Connected;
        onAfterConnect_();
        return true;
    }
    else
    {
        LOG_WRITE( LOG_ERROR, "Failed to connect to " << address_ );
        state_ = ConnectionState::NotConnected;
        return false;
    }
} // Open


void Connection::Close()
{
    if (   ConnectionState::Connected  == state_
        || ConnectionState::Connecting == state_ )
    {
        LOG_WRITE( LOG_INFO, "Disconnecting from " << address_ );
        state_ = ConnectionState::Disconnecting;
        onBeforeDisonnect_();
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
        LOG_WRITE( LOG_ERROR, "Purge " << PurgeFlagsToString( flags ) << " failed: Not connected" );
        return;
    }
    LOG_WRITE( LOG_INFO, "Purge " << PurgeFlagsToString( flags ) );
    PurgeImpl( flags );
} // Purge


void Connection::SetOnAfterConnect( const std::function< void() >& hook )
{
    LOG_WRITE( LOG_INFO, "Set OnAfterConnect hook" );
    onAfterConnect_ = hook;
} // SetOnAfterConnect


void Connection::SetOnBeforeDisonnect( const std::function< void() >& hook )
{
    LOG_WRITE( LOG_INFO, "Set OnBeforeDisconnect hook" );
    onBeforeDisonnect_ = hook;
} // SetOnBeforeDisonnect


void Connection::ResetStatistics()
{
    LOG_WRITE( LOG_INFO, "Reset statistics" );
    txBytesCount_ = 0;
    rxBytesCount_ = 0;
} // ResetStatistics


uint32_t Connection::Read( ByteVector& buffer, uint32_t needed )
{
    uint32_t readed = 0;

    if ( buffer.size() < buffer.capacity() - needed )
    {
        const size_t newSize = buffer.size() + needed;
        LOG_WRITE( LOG_DEBUG, "Reserving new buffer size: " << newSize );
        buffer.reserve( newSize );
    }

    while ( readed < needed )
    {
        if ( !IsConnected() )
        {
            LOG_WRITE( LOG_INFO, "Connection not established. Readed " << readed << " bytes" );
            break;
        }
        const uint32_t readCurrent = ReadImpl( buffer, needed - readed );
        if ( 0 == readCurrent )
        {
            LOG_WRITE( LOG_ERROR, "Read error. Readed " << readed << " bytes" );
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

    while ( written < buffer.size() )
    {
        if ( !IsConnected() )
        {
            LOG_WRITE( LOG_INFO, "Connection not established. Written " << written << " bytes" );
            break;
        }
        const uint32_t writeCurrent = WriteImpl( buffer, written );
        if ( 0 == writeCurrent )
        {
            LOG_WRITE( LOG_ERROR, "Write error. Written " << written << " bytes" );
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


uint32_t Connection::ReadImpl( ByteVector& buffer, uint32_t needed )
{
    ( void ) buffer;
    ( void ) needed;
    return 0;
} // ReadImpl


uint32_t Connection::WriteImpl( const ByteVector& buffer, uint32_t start )
{
    ( void ) buffer;
    ( void ) start;
    return 0;
} // WriteImpl

} // namespace connections

} // namespace logika

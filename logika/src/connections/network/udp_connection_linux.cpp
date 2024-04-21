/// @file Реализация класса для работы с соединением по UDP для Linux
/// @copyright HypeRRu 2024

#include <logika/connections/network/udp_connection.h>
#include <logika/connections/common/linux_io.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

/// @cond
#include <cstring>

#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
/// @endcond

namespace logika
{

namespace connections
{

bool UdpConnection::OpenImpl()
{
    std::string portStr = std::to_string( serverPort_ );
    NetworkAddressInfo hints;
    NetworkAddressInfo* addrinfo;
    NetworkAddressInfo* info;

    std::memset( reinterpret_cast< void* >( &hints ), 0, sizeof( hints ) );
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_DGRAM;
    hints.ai_protocol   = IPPROTO_UDP;
    /// Получение списка структур адресов
    int result = getaddrinfo( serverHostName_.c_str(), portStr.c_str(), &hints, &addrinfo );
    if ( 0 != result )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Failed to get server address info: " )
            << ToLocString( gai_strerror( result ) ) );
        return false;
    }
    /// Открываем соединение
    for ( info = addrinfo; info != nullptr; info = info->ai_next )
    {
        socket_ = socket( info->ai_family, info->ai_socktype | SOCK_NONBLOCK, info->ai_protocol );
        if ( LOGIKA_SOCKET_INVALID == socket_ )
        {
            continue; /// Не удалось открыть сокет с данными параметрами
        }
        if ( -1 != connect( socket_, info->ai_addr, info->ai_addrlen ) )
        {
            struct sockaddr_in* ai = reinterpret_cast< struct sockaddr_in* >( info->ai_addr );
            LOG_WRITE( LOG_INFO, LOCALIZED( "Connected to " ) << locAddress_ );
            freeaddrinfo( info ); /// Больше не используется
            return true;
        }
        /// Не удалось подключится к серверу
        close( socket_ );
        socket_ = LOGIKA_SOCKET_INVALID;
    }
    LOG_WRITE( LOG_ERROR, LOCALIZED( "Can't open connection with " ) << locAddress_ );
    return false;
} // OpenImpl


void UdpConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, LOCALIZED( "Closing connection with " ) << locAddress_ );  
    if ( LOGIKA_SOCKET_INVALID != socket_ )
    {
        shutdown( socket_, SHUT_RDWR );
        close( socket_ );
        socket_ = LOGIKA_SOCKET_INVALID;
    }
} // CloseImpl


void UdpConnection::PurgeImpl( PurgeFlags::Type flags )
{
    using namespace std::placeholders;
    if ( flags & PurgeFlags::Rx )
    {
        constexpr size_t flushBufferSize = 16;
        char buffer[ flushBufferSize ];
        while ( recv( socket_, buffer, flushBufferSize, MSG_PEEK ) > 0 )
        {
            LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Have unreceived datagram, flushing" ) );
            recv( socket_, buffer, flushBufferSize, MSG_TRUNC );
        }
    }
    if ( flags & PurgeFlags::Tx )
    {
        /// Тут делать нечего
    }
} // PurgeImpl


uint32_t UdpConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed, Rc::Type* rc )
{
    using namespace std::placeholders;
    linux_io::ReadFunction readFunction = std::bind( recv, _1, _2, _3, 0 );
    return linux_io::ReadBuffer( readFunction, socket_, buffer, start, needed, readTimeout_, rc );
} // ReadImpl


uint32_t UdpConnection::WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    using namespace std::placeholders;
    linux_io::WriteFunction writeFunction = std::bind( send, _1, _2, _3, MSG_NOSIGNAL );
    return linux_io::WriteBuffer( writeFunction, socket_, buffer, start, rc );
} // WriteImpl

} // namespace connections

} // namespace logika

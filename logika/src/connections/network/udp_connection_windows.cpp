/// @file Реализация класса для работы с соединением по UDP для Windows
/// @copyright HypeRRu 2024

#include <logika/connections/network/udp_connection.h>
#include <logika/connections/common/windows_socket_io.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace logika
{

namespace connections
{

/// @todo WSAStartup
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
        LOG_WRITE( LOG_ERROR, "Failed to get server address info: " << result );
        return false;
    }
    /// Открываем соединение
    for ( info = addrinfo; info != nullptr; info = info->ai_next )
    {
        socket_ = socket( info->ai_family, info->ai_socktype, static_cast< int >( info->ai_protocol ) );
        if ( LOGIKA_SOCKET_INVALID == socket_ )
        {
            continue; /// Не удалось открыть сокет с данными параметрами
        }
        if ( -1 != connect( socket_, info->ai_addr, info->ai_addrlen ) )
        {
            /// Задание неблокирующего режима
            u_long mode = 1;
            ioctlsocket(socket_, FIONBIO, &mode);

            LOG_WRITE( LOG_INFO, "Connected to " << address_ );
            freeaddrinfo( info ); /// Больше не используется
            return true;
        }
        /// Не удалось подключится к серверу
        closesocket( socket_ );
        socket_ = LOGIKA_SOCKET_INVALID;
    }
    LOG_WRITE( LOG_ERROR, "Can't open connection with " << address_ );
    return false;
} // OpenImpl


void UdpConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, "Closing connection with " << address_ );  
    if ( LOGIKA_SOCKET_INVALID != socket_ )
    {
        closesocket( socket_ );
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
            LOG_WRITE( LOG_INFO, "Have unreceived datagram, reading" );
            recv( socket_, buffer, flushBufferSize, 0 );
        }
    }
    if ( flags & PurgeFlags::Tx )
    {
        /// Тут делать нечего
    }
} // PurgeImpl


uint32_t UdpConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed )
{
    return windows::ReadBuffer( socket_, buffer, start, needed, readTimeout_ );
} // ReadImpl


uint32_t UdpConnection::WriteImpl( const ByteVector& buffer, uint32_t start )
{
    return windows::WriteBuffer( socket_, buffer, start );
} // WriteImpl

} // namespace connections

} // namespace logika

/// @file Реализация класса для работы с соединением по TCP для Windows
/// @copyright HypeRRu 2024

#include <logika/connections/network/tcp_connection.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

#include <cstring>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace logika
{

namespace connections
{

/// @todo WSAStartup
bool TcpConnection::OpenImpl()
{
    std::string portStr = std::to_string( serverPort_ );
    NetworkAddressInfo hints;
    NetworkAddressInfo* addrinfo;
    NetworkAddressInfo* info;

    std::memset( reinterpret_cast< void* >( &hints ), 0, sizeof( hints ) );
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;
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
        socket_ = socket( info->ai_family, info->ai_socktype | SOCK_NONBLOCK, info->ai_protocol );
        if ( logika::handleInvalid == socket_ )
        {
            continue; /// Не удалось открыть сокет с данными параметрами
        }
        if ( -1 != connect( socket_, info->ai_addr, info->ai_addrlen ) )
        {
            /// Задание неблокирующего режима
            u_long mode = 1;
            ioctlsocket(sock, FIONBIO, &mode);

            LOG_WRITE( LOG_INFO, "Connected to " << address_ );
            freeaddrinfo( info ); /// Больше не используется
            return true;
        }
        /// Не удалось подключится к серверу
        closesocket( socket_ );
        socket_ = logika::handleInvalid;
    }
    LOG_WRITE( LOG_ERROR, "Can't open connection with " << address_ );
    return false;
} // OpenImpl


void TcpConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, "Closing connection with " << address_ );  
    if ( logika::handleInvalid != socket_ )
    {
        closesocket( socket_ );
        socket_ = logika::handleInvalid;
    }
} // CloseImpl


void TcpConnection::PurgeImpl( PurgeFlags::Type flags )
{
    if ( flags & PurgeFlags::Rx )
    {
        constexpr size_t flushBufferSize = 1024;
        char buffer[ flushBufferSize ];
        const int32_t available = windows::BytesAvailable( socket_ );
        uint32_t readed = 0;
        while ( static_cast< int32_t >( readed ) < available )
        {
            LOG_WRITE( LOG_INFO, "Have unreceived packets, flushing" );
            ssize_t rd = recv( socket_, buffer, flushBufferSize, 0 );
            if ( rd <= 0 )
            {
                LOG_WRITE( LOG_ERROR, "Error while reading. Purged " << rd << " bytes" );
                break;
            }
            readed += static_cast< uint32_t >( rd );
        }
    }
    if ( flags & PurgeFlags::Tx )
    {
        /// Тут делать нечего
    }
} // PurgeImpl


uint32_t TcpConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed )
{
    return windows::ReadBuffer( socket_, buffer, start, needed, readTimeout_ );
} // ReadImpl


uint32_t TcpConnection::WriteImpl( const ByteVector& buffer, uint32_t start )
{
    return windows::WriteBuffer( socket_, buffer, start );
} // WriteImpl

} // namespace connections

} // namespace logika

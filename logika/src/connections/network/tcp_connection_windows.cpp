/// @file Реализация класса для работы с соединением по TCP для Windows
/// @copyright HypeRRu 2024

#include <logika/connections/network/tcp_connection.h>
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
        LOG_WRITE( LOG_ERROR, L"Failed to get server address info: " << result );
        return false;
    }
    /// Открываем соединение
    for ( info = addrinfo; info != nullptr; info = info->ai_next )
    {
        socket_ = socket( info->ai_family, info->ai_socktype, info->ai_protocol );
        if ( LOGIKA_SOCKET_INVALID == socket_ )
        {
            continue; /// Не удалось открыть сокет с данными параметрами
        }
        if ( -1 != connect( socket_, info->ai_addr, static_cast< int >( info->ai_addrlen ) ) )
        {
            /// Задание неблокирующего режима
            u_long mode = 1;
            ioctlsocket(socket_, FIONBIO, &mode);

            LOG_WRITE( LOG_INFO, L"Connected to " << ToLocString( address_ ) );
            freeaddrinfo( info ); /// Больше не используется
            return true;
        }
        /// Не удалось подключится к серверу
        closesocket( socket_ );
        socket_ = LOGIKA_SOCKET_INVALID;
    }
    LOG_WRITE( LOG_ERROR, L"Can't open connection with " << ToLocString( address_ ) );
    return false;
} // OpenImpl


void TcpConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, L"Closing connection with " << ToLocString( address_ ) );  
    if ( LOGIKA_SOCKET_INVALID != socket_ )
    {
        closesocket( socket_ );
        socket_ = LOGIKA_SOCKET_INVALID;
    }
} // CloseImpl


void TcpConnection::PurgeImpl( PurgeFlags::Type flags )
{
    if ( flags & PurgeFlags::Rx )
    {
        constexpr size_t flushBufferSize = 1024;
        char buffer[ flushBufferSize ];
        const int32_t available = windows_io::BytesAvailable( socket_ );
        uint32_t readed = 0;
        while ( static_cast< int32_t >( readed ) < available )
        {
            LOG_WRITE_MSG( LOG_INFO, L"Have unreceived packets, flushing" );
            int rd = recv( socket_, buffer, flushBufferSize, 0 );
            if ( rd <= 0 )
            {
                LOG_WRITE( LOG_ERROR, L"Error while reading. Purged " << rd << L" bytes" );
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


uint32_t TcpConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed, Rc::Type* rc )
{
    return windows_io::ReadBuffer( socket_, buffer, start, needed, readTimeout_, rc );
} // ReadImpl


uint32_t TcpConnection::WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    return windows_io::WriteBuffer( socket_, buffer, start, rc );
} // WriteImpl

} // namespace connections

} // namespace logika

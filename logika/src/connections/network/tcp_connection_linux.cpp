/// @file Реализация класса для работы с соединением по TCP для Linux
/// @copyright HypeRRu 2024

#include <logika/connections/network/tcp_connection.h>
#include <logika/connections/common/linux_io.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

/// @cond
#include <cstring>

#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
/// @endcond


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
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Failed to get server address info: " )
            << ToLocString( gai_strerror( result ) ) );
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
        if ( -1 != connect( socket_, info->ai_addr, info->ai_addrlen ) )
        {
            /// Делаем сокет неблокирующим
            int flags = fcntl( socket_, F_GETFL, 0 );
            if ( -1 == fcntl( socket_, F_SETFL, flags | O_NONBLOCK ) )
            {
                LOG_WRITE( LOG_WARNING, LOCALIZED( "Unable to set non-blocking mode: " )
                    << ToLocString( SafeStrError( errno ) ) );
            }
            LOG_WRITE( LOG_INFO, LOCALIZED( "Connected to " ) << ToLocString( address_ ) );
            freeaddrinfo( info ); /// Больше не используется
            return true;
        }
        /// Не удалось подключится к серверу
        close( socket_ );
        socket_ = LOGIKA_SOCKET_INVALID;
    }
    LOG_WRITE( LOG_ERROR, LOCALIZED( "Can't open connection with " ) << ToLocString( address_ ) );
    return false;
} // OpenImpl


void TcpConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, LOCALIZED( "Closing connection with " ) << ToLocString( address_ ) );  
    if ( LOGIKA_SOCKET_INVALID != socket_ )
    {
        shutdown( socket_, SHUT_RDWR );
        close( socket_ );
        socket_ = LOGIKA_SOCKET_INVALID;
    }
} // CloseImpl


void TcpConnection::PurgeImpl( PurgeFlags::Type flags )
{
    if ( flags & PurgeFlags::Rx )
    {
        constexpr size_t flushBufferSize = 1024;
        char buffer[ flushBufferSize ];
        const int32_t available = linux_io::BytesAvailable( socket_ );
        uint32_t readed = 0;
        while ( static_cast< int32_t >( readed ) < available )
        {
            LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Have unreceived packets, flushing" ) );
            ssize_t rd = recv( socket_, buffer, flushBufferSize, 0 );
            if ( rd <= 0 )
            {
                LOG_WRITE( LOG_ERROR, LOCALIZED( "Error while reading. Purged " ) << rd << LOCALIZED( " bytes" ) );
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
    using namespace std::placeholders;
    linux_io::ReadFunction readFunction = std::bind( recv, _1, _2, _3, 0 );
    return linux_io::ReadBuffer( readFunction, socket_, buffer, start, needed, readTimeout_, rc );
} // ReadImpl


uint32_t TcpConnection::WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    using namespace std::placeholders;
    linux_io::WriteFunction writeFunction = std::bind( send, _1, _2, _3, MSG_NOSIGNAL );
    return linux_io::WriteBuffer( writeFunction, socket_, buffer, start, rc );
} // WriteImpl

} // namespace connections

} // namespace logika

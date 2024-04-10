/// @file Реализация функций для работы с вводом/выводом через сокеты в Windows
/// @copyright HypeRRu 2024

#include <logika/connections/common/windows_socket_io.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

/// @cond
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
/// @endcond

namespace logika
{

namespace connections
{

namespace windows_io
{

uint32_t ReadBuffer( SocketType sock, ByteVector& buffer
    , uint32_t start, uint32_t needed, uint32_t timeout, Rc::Type* rc )
{
    if ( LOGIKA_SOCKET_INVALID == sock )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Invalid connection socket" ) );
        if ( rc )
        {
            *rc = Rc::ConnectionNotSetError;
        }
        return 0;
    }
    if ( buffer.size() < start + needed )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Buffer size (" ) << buffer.size() << LOCALIZED( ") is less " )
                              << LOCALIZED( "than end position (" ) << start + needed << LOCALIZED( ")" ) );
        if ( rc )
        {
            *rc = Rc::InvalidArgError;
        }
        return 0;
    }

    WSAPOLLFD pfd;
    pfd.fd = sock;
    pfd.events = POLLRDNORM;
    
    int readed;
    do
    {
        int ready = WSAPoll( &pfd, 1, 0 != timeout ? static_cast< int >( timeout ) : -1 );
        if ( -1 == ready )
        {
            if ( WSAGetLastError() & WSAEINTR )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Poll failed: " ) << WSAGetLastError() );
            if ( rc )
            {
                *rc = Rc::PollError;
            }
            return 0;
        }
        if ( 0 == ready ) /// Истекло время ожидания данных
        {
            LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Read timeout expired" ) );
            if ( rc )
            {
                *rc = Rc::TimeOutError;
            }
            return 0;
        }

        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Read started" ) );
        readed = recv( sock, &buffer[ start ], needed, 0 );
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Read finished" ) );
        if ( -1 == readed )
        {
            if ( WSAGetLastError() & WSAEWOULDBLOCK )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Read failed: " ) << WSAGetLastError() );
            if ( rc )
            {
                *rc = Rc::ReadError;
            }
            return 0;
        }
        else /// 0 или более байтов считано
        {
            break;
        }
    } while ( true );
    
    LOG_WRITE( LOG_INFO, LOCALIZED( "Readed " ) << readed << LOCALIZED( " bytes" ) );
    if ( rc )
    {
        *rc = Rc::Success;
    }
    return static_cast< uint32_t >( readed );
} // ReadBuffer


uint32_t WriteBuffer( SocketType sock, const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    if ( LOGIKA_SOCKET_INVALID == sock )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Invalid connection socket" ) );
        if ( rc )
        {
            *rc = Rc::ConnectionNotSetError;
        }
        return 0;
    }
    if ( buffer.size() <= start )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Buffer size (" ) << buffer.size() << LOCALIZED( ") is less or equal " )
                              << LOCALIZED( "than start position (" ) << start << LOCALIZED( ")" ) );
        if ( rc )
        {
            *rc = Rc::InvalidArgError;
        }
        return 0;
    }

    int written;
    do
    {
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Write started" ) );
        written = send( sock, &buffer[ start ], static_cast< int >( buffer.size() - start ), 0 );
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Write finished" ) );
        if ( -1 == written )
        {
            if ( WSAGetLastError() & WSAEWOULDBLOCK )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Write failed: " ) << WSAGetLastError() );
            if ( rc )
            {
                *rc = Rc::WriteError;
            }
            return 0;
        }
        else /// 0 или более байтов записано
        {
            break;
        }
    } while ( true );
    
    LOG_WRITE( LOG_INFO, LOCALIZED( "Written " ) << written << LOCALIZED( " bytes" ) );
    if ( rc )
    {
        *rc = Rc::Success;
    }
    return static_cast< uint32_t >( written );
} // WriteBuffer


int32_t BytesAvailable( SocketType sock )
{
    if ( LOGIKA_SOCKET_INVALID == sock )
    {
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Invalid connection socket" ) );
        return 0;
    }

    u_long available;
    if ( -1 == ioctlsocket( sock, FIONREAD, &available ) )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to get bytes available: " ) << WSAGetLastError() );
        return -1;
    }
    return static_cast< int32_t >( available );
} // BytesAvailable

} // namespace windows_io

} // namespace connections

} // namespace logika

/// @file Реализация функций для работы с вводом/выводом через сокеты в Windows
/// @copyright HypeRRu 2024

#include <logika/connections/common/windows_socket_io.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace logika
{

namespace connections
{

namespace windows_io
{

uint32_t ReadBuffer( SocketType sock, ByteVector& buffer
    , uint32_t start, uint32_t needed, uint32_t timeout )
{
    if ( LOGIKA_SOCKET_INVALID == sock )
    {
        LOG_WRITE( LOG_ERROR, "Invalid connection socket" );
        return 0;
    }
    if ( buffer.size() < start + needed )
    {
        LOG_WRITE( LOG_ERROR, "Buffer size (" << buffer.size() << ") is less"
                              << "than end position (" << start + needed << ")" );
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
            LOG_WRITE( LOG_ERROR, "Poll failed: " << WSAGetLastError() );
            return 0;
        }
        if ( 0 == ready ) /// Истекло время ожидания данных
        {
            LOG_WRITE( LOG_WARNING, "Read timeout expired" );
            return 0;
        }

        LOG_WRITE( LOG_DEBUG, "Read started" );
        readed = recv( sock, &buffer[ start ], needed, 0 );
        LOG_WRITE( LOG_DEBUG, "Read finished" );
        if ( -1 == readed )
        {
            if ( WSAGetLastError() & WSAEWOULDBLOCK )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, "Read failed: " << WSAGetLastError() );
            return 0;
        }
        else /// 0 или более байтов считано
        {
            break;
        }
    } while ( true );
    
    LOG_WRITE( LOG_INFO, "Readed " << readed << " bytes" );
    return readed;
} // ReadBuffer


uint32_t WriteBuffer( SocketType sock, const ByteVector& buffer, uint32_t start )
{
    if ( LOGIKA_SOCKET_INVALID == sock )
    {
        LOG_WRITE( LOG_ERROR, "Invalid connection socket" );
        return 0;
    }
    if ( buffer.size() <= start )
    {
        LOG_WRITE( LOG_ERROR, "Buffer size (" << buffer.size() << ") is less or equal "
                              << "than start position (" << start << ")" );
        return 0;
    }

    int written;
    do
    {
        LOG_WRITE( LOG_DEBUG, "Write started" );
        written = send( sock, &buffer[ start ], static_cast< int >( buffer.size() - start ), 0 );
        LOG_WRITE( LOG_DEBUG, "Write finished" );
        if ( -1 == written )
        {
            if ( WSAGetLastError() & WSAEWOULDBLOCK )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, "Write failed: " << WSAGetLastError() );
            return 0;
        }
        else /// 0 или более байтов записано
        {
            break;
        }
    } while ( true );
    
    LOG_WRITE( LOG_INFO, "Written " << written << " bytes" );
    return static_cast< uint32_t >( written );
} // WriteBuffer


int32_t BytesAvailable( SocketType sock )
{
    if ( LOGIKA_SOCKET_INVALID == sock )
    {
        LOG_WRITE( LOG_ERROR, "Invalid connection socket" );
        return 0;
    }

    u_long available;
    if ( -1 == ioctlsocket( sock, FIONREAD, &available ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to get bytes available: " << WSAGetLastError() );
        return -1;
    }
    return available;
} // BytesAvailable

} // namespace windows_io

} // namespace connections

} // namespace logika

/// @file Реализация функций для работы с вводом/выводом в Linux
/// @copyright HypeRRu 2024

#include <logika/connections/common/linux_io.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

#include <poll.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

namespace logika
{

namespace connections
{

namespace linux_io
{

uint32_t ReadBuffer( ReadFunction readfn, FileHandleType handle, ByteVector& buffer
    , uint32_t start, uint32_t needed, uint32_t timeout )
{
    if ( LOGIKA_FILE_HANDLE_INVALID == handle )
    {
        LOG_WRITE( LOG_ERROR, "Invalid connection handle" );
        return 0;
    }
    if ( !readfn )
    {
        LOG_WRITE( LOG_ERROR, "Invalid read function" );
        return 0;
    }
    if ( buffer.size() < start + needed )
    {
        LOG_WRITE( LOG_ERROR, "Buffer size (" << buffer.size() << ") is less"
                              << "than end position (" << start + needed << ")" );
        return 0;
    }

    struct pollfd pfd;
    pfd.fd = handle;
    pfd.events = POLLIN;

    ssize_t readed;
    do
    {
        int ready = poll( &pfd, 1, 0 != timeout ? static_cast< int >( timeout ) : -1 );
        if ( -1 == ready )
        {
            if ( errno & EINTR )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, "Poll failed: " << SafeStrError( errno ) );
            return 0;
        }
        /// @todo Другой RC для таймаута?
        if ( 0 == ready ) /// Истекло время ожидания данных
        {
            LOG_WRITE( LOG_WARNING, "Read timeout expired" );
            return 0;
        }

        LOG_WRITE( LOG_DEBUG, "Read started" );
        readed = readfn( handle, &buffer[ start ], needed );
        LOG_WRITE( LOG_DEBUG, "Read finished" );
        if ( -1 == readed )
        {
            if ( ( errno & EAGAIN ) || ( errno & EWOULDBLOCK ) )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, "Read failed: " << SafeStrError( errno ) );
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


uint32_t WriteBuffer( WriteFunction writefn, FileHandleType handle, const ByteVector& buffer, uint32_t start )
{
    if ( LOGIKA_FILE_HANDLE_INVALID == handle )
    {
        LOG_WRITE( LOG_ERROR, "Invalid connection handle" );
        return 0;
    }
    if ( !writefn )
    {
        LOG_WRITE( LOG_ERROR, "Invalid write function" );
        return 0;
    }
    if ( buffer.size() <= start )
    {
        LOG_WRITE( LOG_ERROR, "Buffer size (" << buffer.size() << ") is less or equal "
                              << "than start position (" << start << ")" );
        return 0;
    }

    ssize_t written;
    do
    {
        LOG_WRITE( LOG_DEBUG, "Write started" );
        written = writefn( handle, &buffer[ start ], buffer.size() - start );
        LOG_WRITE( LOG_DEBUG, "Write finished" );
        if ( -1 == written )
        {
            if ( ( errno & EAGAIN ) || ( errno & EWOULDBLOCK ) )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, "Write failed: " << SafeStrError( errno ) );
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


int32_t BytesAvailable( FileHandleType handle )
{
    if ( LOGIKA_FILE_HANDLE_INVALID == handle )
    {
        LOG_WRITE( LOG_ERROR, "Invalid connection handle" );
        return 0;
    }

    int32_t available;
    if ( -1 == ioctl( handle, FIONREAD, &available ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to get bytes available: " << SafeStrError( errno ) );
        return -1;
    }
    return available;
} // BytesAvailable

} // namespace linux_io

} // namespace connections

} // namespace logika

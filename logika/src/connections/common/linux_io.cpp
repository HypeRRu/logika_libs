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
    , uint32_t start, uint32_t needed, uint32_t timeout, Rc::Type* rc )
{
    if ( LOGIKA_FILE_HANDLE_INVALID == handle )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid connection handle" );
        if ( rc )
        {
            *rc = Rc::ConnectionNotSetError;
        }
        return 0;
    }
    if ( !readfn )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid read function" );
        if ( rc )
        {
            *rc = Rc::InvalidArgError;
        }
        return 0;
    }
    if ( buffer.size() < start + needed )
    {
        LOG_WRITE( LOG_ERROR, L"Buffer size (" << buffer.size() << L") is less "
                              << L"than end position (" << start + needed << L")" );
        if ( rc )
        {
            *rc = Rc::InvalidArgError;
        }
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
            LOG_WRITE( LOG_ERROR, L"Poll failed: " << ToLocString( SafeStrError( errno ) ) );
            if ( rc )
            {
                *rc = Rc::PollError;
            }
            return 0;
        }
        if ( 0 == ready ) /// Истекло время ожидания данных
        {
            LOG_WRITE_MSG( LOG_WARNING, L"Read timeout expired" );
            if ( rc )
            {
                *rc = Rc::TimeOutError;
            }
            return 0;
        }

        LOG_WRITE_MSG( LOG_DEBUG, L"Read started" );
        readed = readfn( handle, &buffer[ start ], needed );
        LOG_WRITE_MSG( LOG_DEBUG, L"Read finished" );
        if ( -1 == readed )
        {
            if ( ( errno & EAGAIN ) || ( errno & EWOULDBLOCK ) )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, L"Read failed: " << ToLocString( SafeStrError( errno ) ) );
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
    
    LOG_WRITE( LOG_INFO, L"Readed " << readed << L" bytes" );
    if ( rc )
    {
        *rc = Rc::Success;
    }
    return static_cast< uint32_t >( readed );
} // ReadBuffer


uint32_t WriteBuffer( WriteFunction writefn, FileHandleType handle, const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    if ( LOGIKA_FILE_HANDLE_INVALID == handle )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid connection handle" );
        if ( rc )
        {
            *rc = Rc::ConnectionNotSetError;
        }
        return 0;
    }
    if ( !writefn )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid write function" );
        if ( rc )
        {
            *rc = Rc::InvalidArgError;
        }
        return 0;
    }
    if ( buffer.size() <= start )
    {
        LOG_WRITE( LOG_ERROR, L"Buffer size (" << buffer.size() << L") is less or equal "
                              << L"than start position (" << start << L")" );
        if ( rc )
        {
            *rc = Rc::InvalidArgError;
        }
        return 0;
    }

    ssize_t written;
    do
    {
        LOG_WRITE_MSG( LOG_DEBUG, L"Write started" );
        written = writefn( handle, &buffer[ start ], buffer.size() - start );
        LOG_WRITE_MSG( LOG_DEBUG, L"Write finished" );
        if ( -1 == written )
        {
            if ( ( errno & EAGAIN ) || ( errno & EWOULDBLOCK ) )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, L"Write failed: " << ToLocString( SafeStrError( errno ) ) );
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
    
    LOG_WRITE( LOG_INFO, L"Written " << written << L" bytes" );
    if ( rc )
    {
        *rc = Rc::Success;
    }
    return static_cast< uint32_t >( written );
} // WriteBuffer


int32_t BytesAvailable( FileHandleType handle )
{
    if ( LOGIKA_FILE_HANDLE_INVALID == handle )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid connection handle" );
        return 0;
    }

    int32_t available;
    if ( -1 == ioctl( handle, FIONREAD, &available ) )
    {
        LOG_WRITE( LOG_ERROR, L"Unable to get bytes available: " << ToLocString( SafeStrError( errno ) ) );
        return -1;
    }
    return available;
} // BytesAvailable

} // namespace linux_io

} // namespace connections

} // namespace logika

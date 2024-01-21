/// @file Реализация класса для работы с соединением по COM-порту для Linux
/// @copyright HypeRRu 2024
/// @see serial_port_connection.cpp

#include <logika/connections/serial/serial_port_connection.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

#include <cstring>

#include <poll.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

/// @todo Сделать обновление опций без переподключения
/// @todo Убрать лишние проверки в Read/Write
namespace logika
{

namespace connections
{

bool SerialPortConnection::OpenImpl()
{
    /// Проверка настроек соединения
    if ( !IsSettingsValid() )
    {
        LOG_WRITE( LOG_ERROR, "Connection options invalid" );
        return false;
    }

    /// Настройки устройства
    struct termios options;
    bzero( &options, sizeof( options ) );
    /// Установка скорости работы
    if ( -1 == cfsetispeed( &options, baudRate_ ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to set device speed: " << SafeStrError( errno ) );
        return false;
    }
    /// Конфигурация устройства
    options.c_cflag |= CLOCAL | CREAD | stopBits_ | dataBits_ | parity_;
    options.c_iflag |= IGNPAR | IGNBRK;
    /// Таймер не используется
    options.c_cc[ VTIME ] = 0;
    /// Минимальное количество байтов для чтения - 1
    options.c_cc[ VMIN ]  = 1;

    /// Открытие соединения с устройством
    handle_ = open( address_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
    if ( logika::handleInvalid == handle_ )
    {
        LOG_WRITE( LOG_ERROR, "Can't open device: " << SafeStrError( errno ) );
        return false;
    }
    /// Установка неблокирующего режима
    int flags = fcntl( handle_, F_GETFL, 0 );
	if ( -1 == fcntl( handle_, F_SETFL, flags | O_NONBLOCK ) )
    {
        LOG_WRITE( LOG_WARNING, "Unable to set device mode to non-blocking" )
    }
    /// Применение конфигурации
    if ( -1 == tcsetattr( handle_, TCSANOW, &options ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to set device settings: " << SafeStrError( errno ) );
        CloseImpl();
        return false;
    }

    LOG_WRITE( LOG_INFO, "Connected successfully to " << address_ );
    return true;
} // OpenImpl


void SerialPortConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, "Closing connection to device " << address_ );
    close( handle_ );
    handle_ = logika::handleInvalid;
} // CloseImpl


void SerialPortConnection::PurgeImpl( PurgeFlags::Type flags )
{
    if ( flags & PurgeFlags::Rx )
    {
        tcflush( handle_, TCIFLUSH );
    }
    if ( flags & PurgeFlags::Tx )
    {
        tcflush( handle_, TCOFLUSH );
    }
} // PurgeImpl


uint32_t SerialPortConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed )
{
    if ( !IsConnected() )
    {
        return 0;
    }
    if ( buffer.size() < start + needed )
    {
        LOG_WRITE( LOG_ERROR, "Buffer size (" << buffer.size() << ") is less"
                              << "than end position (" << start + needed << ")" );
        return 0;
    }

    struct pollfd pfd;
    pfd.fd = handle_;
    pfd.events = POLLIN;

    ssize_t readed;
    do
    {
        int ready = poll( &pfd, 1, 0 != readTimeout_ ? static_cast< int >( readTimeout_ ) : -1 );
        if ( -1 == ready )
        {
            if ( errno & EINTR )
            {
                continue;
            }
            LOG_WRITE( LOG_ERROR, "Poll failed: " << SafeStrError( errno ) );
            return 0;
        }
        /// @todo Другой RC для таймаута
        if ( 0 == ready ) /// Истекло время ожидания данных
        {
            LOG_WRITE( LOG_WARNING, "Read timeout expired" );
            return 0;
        }

        readed = read( handle_, &buffer[ start ], needed );
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
    
    LOG_WRITE( LOG_INFO, "Readed " << readed << " bytes from " << address_ );
    return readed;
} // ReadImpl


uint32_t SerialPortConnection::WriteImpl( const ByteVector& buffer, uint32_t start )
{
    if ( !IsConnected() )
    {
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
        written = write( handle_, &buffer[ start ], buffer.size() - start );
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
    
    LOG_WRITE( LOG_INFO, "Written " << written << " bytes to " << address_ );
    return static_cast< uint32_t >( written );
} // WriteImpl


int32_t SerialPortConnection::BytesAvailable()
{
    if ( IsConnected() )
    {
        LOG_WRITE( LOG_ERROR, "Connection not established" );
        return 0;
    }

    int32_t available;
    if ( -1 == ioctl( handle_, FIONREAD, &available ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to get bytes available: " << SafeStrError( errno ) );
        return -1;
    }
    return available;
} // BytesAvailable

} // namespace connections

} // namespace logika

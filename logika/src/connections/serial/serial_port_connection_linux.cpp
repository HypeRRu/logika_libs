/// @file Реализация класса для работы с соединением по COM-порту для Linux
/// @copyright HypeRRu 2024
/// @see serial_port_connection.cpp

#include <logika/connections/serial/serial_port_connection.h>
#include <logika/connections/common/linux_io.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

/// @cond
#include <cstring>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
/// @endcond

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
        LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Connection options invalid" ) );
        return false;
    }

    /// Настройки устройства
    struct termios options;
    bzero( &options, sizeof( options ) );
    /// Установка скорости работы
    if ( -1 == cfsetispeed( &options, baudRate_ ) )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to set device speed: " )
            << ToLocString( SafeStrError( errno ) ) );
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
    if ( LOGIKA_FILE_HANDLE_INVALID == handle_ )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Can't open device: " ) << ToLocString( SafeStrError( errno ) ) );
        return false;
    }
    /// Установка неблокирующего режима
    int flags = fcntl( handle_, F_GETFL, 0 );
	if ( -1 == fcntl( handle_, F_SETFL, flags | O_NONBLOCK ) )
    {
        LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Unable to set device mode to non-blocking" ) )
    }
    /// Применение конфигурации
    if ( -1 == tcsetattr( handle_, TCSANOW, &options ) )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to set device settings: " )
            << ToLocString( SafeStrError( errno ) ) );
        CloseImpl();
        return false;
    }

    LOG_WRITE( LOG_INFO, LOCALIZED( "Connected successfully to " ) << ToLocString( address_ ) );
    return true;
} // OpenImpl


void SerialPortConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, LOCALIZED( "Closing connection to device " ) << ToLocString( address_ ) );
    if ( LOGIKA_FILE_HANDLE_INVALID != handle_ )
    {
        close( handle_ );
        handle_ = LOGIKA_FILE_HANDLE_INVALID;
    }
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


uint32_t SerialPortConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed, Rc::Type* rc )
{
    return linux_io::ReadBuffer( read, handle_, buffer, start, needed, readTimeout_, rc );
} // ReadImpl


uint32_t SerialPortConnection::WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    return linux_io::WriteBuffer( write, handle_, buffer, start, rc );
} // WriteImpl


int32_t SerialPortConnection::BytesAvailable()
{
    return linux_io::BytesAvailable( handle_ );
} // BytesAvailable

} // namespace connections

} // namespace logika

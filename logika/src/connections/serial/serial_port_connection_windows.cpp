/// @file Реализация класса для работы с соединением по COM-порту для Windows
/// @copyright HypeRRu 2024
/// @see serial_port_connection.cpp

#include <logika/connections/serial/serial_port_connection.h>

#include <logika/log/defines.h>

#include <system_error>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/// @todo Проверить данный код
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
    /// Открытие соединения с устройством
    handle_ = CreateFileA( address_.c_str()
        , GENERIC_READ | GENERIC_WRITE
        , 0, 0, OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL, 0 );
    if ( LOGIKA_FILE_HANDLE_INVALID == handle_ )
    {
        LOG_WRITE( LOG_ERROR, "Can't open device " << address_
                              << ": " << std::system_category().message( GetLastError() ) );
        return false;
    }

    /// Настройки устройства
    DCB options;
    options.DCBlength = sizeof( DCB );
    /// Получение текущих настроек устройства
    if ( !GetCommState( handle_, &options ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to get device options for " << address_ );
        CloseImpl();
        return false;
    }
    /// Установка настроек устройства
    options.BaudRate        = baudRate_;
    options.Parity          = parity_;
    options.ByteSize        = dataBits_;
    options.StopBits        = stopBits_;

    options.fOutxCtsFlow    = false;
    options.fOutxDsrFlow    = false;
    options.fOutX           = false;
    options.fDtrControl     = DTR_CONTROL_DISABLE;
    options.fRtsControl     = RTS_CONTROL_DISABLE;
    /// Применение конфигурации
    if ( !SetCommState( handle_, &options ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to set device options for " << address_ );
        CloseImpl();
        return false;
    }
    /// Настройка таймаутов для операций чтения и записи
    /// @see https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-commtimeouts
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout            = MAXDWORD;     ///< Интервал между поступлением двух символов
    timeouts.ReadTotalTimeoutConstant       = readTimeout_; ///< Время ожидания поступления символов, мс
    timeouts.ReadTotalTimeoutMultiplier     = MAXDWORD;     ///< Множитель для вычисления полного времени простоя чтения
    timeouts.WriteTotalTimeoutConstant      = 0;            ///< Время ожидания записи символов, мс
    timeouts.WriteTotalTimeoutMultiplier    = 0;            ///< Множитель для вычисления полного времени простоя записи

    if ( !SetCommTimeouts( handle_, &timeouts ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to set RW timeouts for " << address_ );
        CloseImpl();
        return false;
    }

    LOG_WRITE( LOG_INFO, "Connected successfully to " << address_ );
    return true;
} // OpenImpl


void SerialPortConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, "Closing connection to device " << address_ );  
    if ( LOGIKA_FILE_HANDLE_INVALID == handle_ )
    {
        CloseHandle( handle_ );
        handle_ = LOGIKA_FILE_HANDLE_INVALID;
    }
} // CloseImpl


void SerialPortConnection::PurgeImpl( PurgeFlags::Type flags )
{
    if ( flags & PurgeFlags::Rx )
    {
        PurgeComm( handle_, PURGE_RXCLEAR );
    }
    if ( flags & PurgeFlags::Tx )
    {
        PurgeComm( handle_, PURGE_TXCLEAR );
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

    DWORD readed = 0;
    if ( !ReadFile( handle_, &buffer[ start ], needed, &readed, nullptr ) )
    {
        LOG_WRITE( LOG_ERROR, "Read failed: " << std::system_category().message( GetLastError() ) );
        return 0;
    }
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

    DWORD written = 0;
    if ( !WriteFile( handle_, &buffer[ start ], buffer.size() - start, &written, nullptr ) )
    {
        LOG_WRITE( LOG_ERROR, "Write failed: " << std::system_category().message( GetLastError() ) );
        return 0;
    }
    return written;
} // WriteImpl


int32_t SerialPortConnection::BytesAvailable()
{
    if ( IsConnected() )
    {
        LOG_WRITE( LOG_ERROR, "Connection not established" );
        return -1;
    }

    DWORD errors;
    COMSTAT status;
    if ( !ClearCommError( handle_, &errors, &status ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to get bytes available" );
        return -1;
    }
    return status.cbInQue;
} // BytesAvailable

} // namespace connections

} // namespace logika

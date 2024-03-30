/// @file Реализация класса для работы с соединением по COM-порту для Windows
/// @copyright HypeRRu 2024
/// @see serial_port_connection.cpp

#include <logika/connections/serial/serial_port_connection.h>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

/// @cond
#include <system_error>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
/// @endcond


namespace logika
{

namespace connections
{

bool SerialPortConnection::OpenImpl()
{
    /// Проверка настроек соединения
    if ( !IsSettingsValid() )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Connection options invalid" );
        return false;
    }
    /// Открытие соединения с устройством
    handle_ = CreateFileA( address_.c_str()
        , GENERIC_READ | GENERIC_WRITE
        , 0, 0, OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL, 0 );
    if ( LOGIKA_FILE_HANDLE_INVALID == handle_ )
    {
        LOG_WRITE( LOG_ERROR, L"Can't open device " << ToLocString( address_ )
                              << L": " << GetLastError() );
        return false;
    }

    /// Настройки устройства
    DCB options;
    options.DCBlength = sizeof( DCB );
    /// Получение текущих настроек устройства
    if ( !GetCommState( handle_, &options ) )
    {
        LOG_WRITE( LOG_ERROR, L"Unable to get device options for " << ToLocString( address_ ) );
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
        LOG_WRITE( LOG_ERROR, L"Unable to set device options for " << ToLocString( address_ ) );
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
        LOG_WRITE( LOG_ERROR, L"Unable to set RW timeouts for " << ToLocString( address_ ) );
        CloseImpl();
        return false;
    }

    LOG_WRITE( LOG_INFO, L"Connected successfully to " << ToLocString( address_ ) );
    return true;
} // OpenImpl


void SerialPortConnection::CloseImpl()
{
    LOG_WRITE( LOG_INFO, L"Closing connection to device " << ToLocString( address_ ) );  
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


uint32_t SerialPortConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed, Rc::Type* rc )
{
    if ( !IsConnected() )
    {
        if ( rc )
        {
            *rc = Rc::ConnectionNotSetError;
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

    DWORD readed = 0;
    if ( !ReadFile( handle_, &buffer[ start ], needed, &readed, nullptr ) )
    {
        LOG_WRITE( LOG_ERROR, L"Read failed: " << GetLastError() );
        if ( rc )
        {
            if ( GetLastError() == WAIT_TIMEOUT )
            {
                *rc = Rc::TimeOutError;
            }
            else
            {
                *rc = Rc::ReadError;
            }
        }
        return 0;
    }

    if ( rc )
    {
        *rc = Rc::Success;
    }
    return static_cast< uint32_t >( readed );
} // ReadImpl


uint32_t SerialPortConnection::WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc )
{
    if ( !IsConnected() )
    {
        if ( rc )
        {
            *rc = Rc::ConnectionNotSetError;
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

    DWORD written = 0;
    if ( !WriteFile( handle_, &buffer[ start ], static_cast< DWORD >( buffer.size() - start ), &written, nullptr ) )
    {
        LOG_WRITE( LOG_ERROR, L"Write failed: " << GetLastError() );
        if ( rc )
        {
            *rc = Rc::WriteError;
        }
        return 0;
    }

    if ( rc )
    {
        *rc = Rc::Success;
    }
    return static_cast< uint32_t >( written );
} // WriteImpl


int32_t SerialPortConnection::BytesAvailable()
{
    if ( IsConnected() )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Connection not established" );
        return -1;
    }

    DWORD errors;
    COMSTAT status;
    if ( !ClearCommError( handle_, &errors, &status ) )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Unable to get bytes available" );
        return -1;
    }
    return status.cbInQue;
} // BytesAvailable

} // namespace connections

} // namespace logika

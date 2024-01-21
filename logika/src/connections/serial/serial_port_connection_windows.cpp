/// @file Реализация класса для работы с соединением по COM-порту для Windows
/// @copyright HypeRRu 2024
/// @see serial_port_connection.cpp

#include <logika/connections/serial/serial_port_connection.h>

#include <logika/log/defines.h>

#include <windows.h>


namespace logika
{

namespace connections
{

bool SerialPortConnection::OpenImpl()
{
    /// Проверка настроек соединения
    if ( address_.empty()
        || baudRate_ == BaudRate::NotSupported
        || stopBits_ == StopBits::NotSupported
        || dataBits_ == DataBits::NotSupported
        || parity_   == Parity::NotSupported )
    {
        LOG_WRITE( LOG_ERROR, "Connection options invalid" );
        return false;
    }
} // OpenImpl


void SerialPortConnection::CloseImpl()
{

} // CloseImpl


void SerialPortConnection::PurgeImpl( PurgeFlags::Type flags )
{

} // PurgeImpl


uint32_t SerialPortConnection::ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed )
{
    return 0;
} // ReadImpl


uint32_t SerialPortConnection::WriteImpl( const ByteVector& buffer, uint32_t start )
{
    return 0;
} // WriteImpl

} // namespace connections

} // namespace logika

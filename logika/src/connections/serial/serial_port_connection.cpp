/// @file Реализация класса для работы с соединением по COM-порту
/// @copyright HypeRRu 2024
/// @see serial_port_connection_linux.cpp
/// @see serial_port_connection_windows.cpp

#include <logika/connections/serial/serial_port_connection.h>

#include <logika/log/defines.h>


namespace logika
{

namespace connections
{

SerialPortConnection::SerialPortConnection( const std::string& portName, uint32_t readTimeout )
    : SerialConnection( portName, readTimeout )
    , handle_{ logika::handleInvalid }
{
    type_ = ConnectionType::ComPort;
} // SerialPortConnection( const std::string& portName, uint32_t readTimeout )


SerialPortConnection::SerialPortConnection(
      const std::string& portName, BaudRate::Type baudRate
    , StopBits::Type stopBits, DataBits::Type dataBits
    , Parity::Type parity, uint32_t readTimeout )
    : SerialConnection( portName, readTimeout )
    , handle_{ logika::handleInvalid }
{
    type_ = ConnectionType::ComPort;

    SetBaudRate( baudRate );
    SetStopBits( stopBits );
    SetDataBits( dataBits );
    SetParity( parity );
} // SerialPortConnection( const std::string& portName, BaudRate::Type baudRate, ... )

} // namespace connections

} // namespace logika

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

SerialPortConnection::SerialPortConnection( const std::string& portName, TimeType readTimeout )
    : SerialConnection( portName, readTimeout )
    , handle_{ LOGIKA_FILE_HANDLE_INVALID }
{
    type_ = ConnectionType::ComPort;
} // SerialPortConnection( const std::string& portName, TimeType readTimeout )


SerialPortConnection::SerialPortConnection(
      const std::string& portName, BaudRate::Type baudRate
    , StopBits::Type stopBits, DataBits::Type dataBits
    , Parity::Type parity, TimeType readTimeout )
    : SerialConnection( portName, readTimeout )
    , handle_{ LOGIKA_FILE_HANDLE_INVALID }
{
    type_ = ConnectionType::ComPort;

    SetBaudRate( baudRate );
    SetStopBits( stopBits );
    SetDataBits( dataBits );
    SetParity( parity );
} // SerialPortConnection( const std::string& portName, BaudRate::Type baudRate, ... )


SerialPortConnection::~SerialPortConnection()
{
    Close();
} // ~SerialPortConnection


bool SerialPortConnection::IsSettingsValid() const
{
    return ( !address_.empty()
        && baudRate_ != BaudRate::NotSupported
        && stopBits_ != StopBits::NotSupported
        && dataBits_ != DataBits::NotSupported
        && parity_   != Parity::NotSupported );
} // IsSettingsValid

} // namespace connections

} // namespace logika

/// @file Реализация базового класса для работы с соединением по последовательному порту
/// @copyright HypeRRu 2024

#include <logika/connections/serial/serial_connection.h>
#include <logika/connections/utils/types_converter.h>

#include <logika/log/defines.h>

namespace logika
{

namespace connections
{

SerialConnection::SerialConnection( const std::string& portName, uint32_t readTimeout )
    : Connection( portName, readTimeout )
    , baudRate_{ BaudRate::NotSupported }
    , stopBits_{ StopBits::NotSupported }
    , dataBits_{ DataBits::NotSupported }
    , parity_{ Parity::NotSupported }
{
    type_ = ConnectionType::Serial;
} // SerialConnection


BaudRate::Type SerialConnection::GetBaudRate() const
{
    return baudRate_;
} // GetBaudRate


void SerialConnection::SetBaudRate( BaudRate::Type rate )
{
    LOG_WRITE( LOG_INFO, "Set port " << address_ << " baud rate to " << BaudRateToString( rate ) );
    baudRate_ = rate;
} // SetBaudRate


StopBits::Type SerialConnection::GetStopBits() const
{
    return stopBits_;
} // GetStopBits


void SerialConnection::SetStopBits( StopBits::Type bits )
{
    LOG_WRITE( LOG_INFO, "Set port " << address_ << " stop bits to " << StopBitsToString( bits ) );
    stopBits_ = bits;
} // SetStopBits


DataBits::Type SerialConnection::GetDataBits() const
{
    return dataBits_;
} // GetDataBits


void SerialConnection::SetDataBits( DataBits::Type bits )
{
    LOG_WRITE( LOG_INFO, "Set port " << address_ << " data bits to " << DataBitsToString( bits ) );
    dataBits_ = bits;
} // SetDataBits


Parity::Type SerialConnection::GetParity() const
{
    return parity_;
} // GetParity


void SerialConnection::SetParity( Parity::Type parity )
{
    LOG_WRITE( LOG_INFO, "Set port " << address_ << " parity to " << ParityToString( parity ) );
    parity_ = parity;
} // SetParity

} // namespace connections

} // namespace logika

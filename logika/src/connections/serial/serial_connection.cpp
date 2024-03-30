/// @file Реализация базового класса для работы с соединением по последовательному порту
/// @copyright HypeRRu 2024

#include <logika/connections/serial/serial_connection.h>
#include <logika/connections/utils/types_converter.h>
#include <logika/connections/utils/types_checker.hpp>

#include <logika/log/defines.h>
#include <logika/common/misc.h>

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


bool SerialConnection::SetBaudRate( BaudRate::Type rate )
{
    LOG_WRITE( LOG_INFO, L"Set port " << ToLocString( address_ )
        << L" baud rate to " << ToLocString( BaudRateToString( rate ) ) );
    if ( !IsValidBaudRate( rate ) )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid baud rate" );
        baudRate_ = BaudRate::NotSupported;
        Close();
        return false;
    }
    
    baudRate_ = rate;
    if ( IsConnected() )
    {
        LOG_WRITE( LOG_INFO, L"Port " << ToLocString( address_ )
            << L" settings changed when connection is opened. Reopenning connection" )
        Open(); /// Переподключение после изменения настроек
    }
    return true;
} // SetBaudRate


StopBits::Type SerialConnection::GetStopBits() const
{
    return stopBits_;
} // GetStopBits


bool SerialConnection::SetStopBits( StopBits::Type bits )
{
    LOG_WRITE( LOG_INFO, L"Set port " << ToLocString( address_ )
        << L" stop bits to " << ToLocString( StopBitsToString( bits ) ) );
    if ( !IsValidStopBits( bits ) )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid stop bits" );
        stopBits_ = StopBits::NotSupported;
        Close();
        return false;
    }

    stopBits_ = bits;
    if ( IsConnected() )
    {
        LOG_WRITE( LOG_INFO, L"Port " << ToLocString( address_ )
            << L" settings changed when connection is opened. Reopenning connection" )
        Open(); /// Переподключение после изменения настроек
    }

    return true;
} // SetStopBits


DataBits::Type SerialConnection::GetDataBits() const
{
    return dataBits_;
} // GetDataBits


bool SerialConnection::SetDataBits( DataBits::Type bits )
{
    LOG_WRITE( LOG_INFO, L"Set port " << ToLocString( address_ )
        << L" data bits to "<< ToLocString( DataBitsToString( bits ) ) );
    if ( !IsValidDataBits( bits ) )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid data bits" );
        dataBits_ = DataBits::NotSupported;
        Close();
        return false;
    }

    dataBits_ = bits;
    if ( IsConnected() )
    {
        LOG_WRITE( LOG_INFO, L"Port " << ToLocString( address_ )
            << L" settings changed when connection is opened. Reopenning connection" )
        Open(); /// Переподключение после изменения настроек
    }
    return true;
} // SetDataBits


Parity::Type SerialConnection::GetParity() const
{
    return parity_;
} // GetParity


bool SerialConnection::SetParity( Parity::Type parity )
{
    LOG_WRITE( LOG_INFO, L"Set port " << ToLocString( address_ )
        << L" parity to " << ToLocString( ParityToString( parity ) ) );
    if ( !IsValidParity( parity ) )
    {
        LOG_WRITE_MSG( LOG_ERROR, L"Invalid parity" );
        parity_ = Parity::NotSupported;
        Close();
        return false;
    }

    parity_ = parity;
    if ( IsConnected() )
    {
        LOG_WRITE( LOG_INFO, L"Port " << ToLocString( address_ )
            << L" settings changed when connection is opened. Reopenning connection" )
        Open(); /// Переподключение после изменения настроек
    }
    return true;
} // SetParity

} // namespace connections

} // namespace logika

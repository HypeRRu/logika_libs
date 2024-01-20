/// @file Реализация функций для конвертации типов соединений
/// @copyright HypeRRu 2024

#include <logika/connections/utils/types_converter.h>

#include <unordered_map>

namespace logika
{

namespace connections
{

std::string ConnectionTypeToString( ConnectionType::Type type )
{
    static const std::unordered_map< ConnectionType::Type, std::string > mapToString{
          { ConnectionType::Offline,    "Offline connection" }
        , { ConnectionType::Serial,     "Serial connection" }
        , { ConnectionType::Network,    "Network connection" }
        , { ConnectionType::ComPort,    "COM-port connection" }
        , { ConnectionType::Tcp,        "TCP connection" }
        , { ConnectionType::Udp,        "UDP connection" }
        , { ConnectionType::Modem,      "Modem connection" }
        , { ConnectionType::Radius,     "Radius connection" }
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "Unknown connection";
} // ConnectionTypeToString


std::string ConnectionStateToString( ConnectionState state )
{
    static const std::unordered_map< ConnectionState, std::string > mapToString{
          { ConnectionState::NotConnected,  "Not connected" }
        , { ConnectionState::Connecting,    "Connecting" }
        , { ConnectionState::Connected,     "Connected" }
        , { ConnectionState::Disconnecting, "Disconnecting" }
    };
    auto converted = mapToString.find( state );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "Unknown state";
} // ConnectionStateToString


std::string PurgeFlagsToString( PurgeFlags::Type flags )
{
    static const std::unordered_map< PurgeFlags::Type, std::string > mapToString{
          { PurgeFlags::Rx,     "RX" }
        , { PurgeFlags::Tx,     "TX" }
        , { PurgeFlags::TxRx,   "RX and TX" }
    };
    auto converted = mapToString.find( flags );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "Unknown";
} // PurgeFlagsToString


std::string BaudRateToString( BaudRate::Type rate )
{
    static const std::unordered_map< BaudRate::Type, std::string > mapToString{
          { BaudRate::NotSupported, "Not supported baud rate" }
        , { BaudRate::Rate110,      "110 bps" }
        , { BaudRate::Rate300,      "300 bps" }
        , { BaudRate::Rate600,      "600 bps" }
        , { BaudRate::Rate1200,     "1200 bps" }
        , { BaudRate::Rate2400,     "2400 bps" }
        , { BaudRate::Rate4800,     "4800 bps" }
        , { BaudRate::Rate9600,     "9600 bps" }
        , { BaudRate::Rate19200,    "19200 bps" }
        , { BaudRate::Rate38400,    "38400 bps" }
        , { BaudRate::Rate57600,    "57600 bps" }
        , { BaudRate::Rate115200,   "115200 bps" }
    };
    auto converted = mapToString.find( rate );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "Unknown baud rate";
} // BaudRateToString


std::string StopBitsToString( StopBits::Type bits )
{
    static const std::unordered_map< StopBits::Type, std::string > mapToString{
          { StopBits::NotSupported,         "Not supported stop bits count" }
        , { StopBits::StopBitOne,           "1 stop bit" }
        , { StopBits::StopBitOneAndHalf,    "1.5 stop bits" }
        , { StopBits::StopBitsTwo,          "2 stop bits" }
    };
    auto converted = mapToString.find( bits );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "Unknown stop bits count";
} // StopBitsToString


std::string DataBitsToString( DataBits::Type bits )
{
    static const std::unordered_map< DataBits::Type, std::string > mapToString{
          { DataBits::NotSupported, "Not supported data bits count" }
        , { DataBits::DataBits5,    "5 data bits" }
        , { DataBits::DataBits6,    "6 data bits" }
        , { DataBits::DataBits7,    "7 data bits" }
        , { DataBits::DataBits8,    "8 data bits" }
    };
    auto converted = mapToString.find( bits );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "Unknown data bits count";
} // DataBitsToString


std::string ParityToString( Parity::Type parity )
{
    static const std::unordered_map< Parity::Type, std::string > mapToString{
          { Parity::NotSupported,   "Not supported parity" }
        , { Parity::ParityNone,     "No parity" }
        , { Parity::ParityEven,     "Even parity" }
        , { Parity::ParityOdd,      "Odd parity" }
        , { Parity::ParityMark,     "Mark parity" }
        , { Parity::ParitySpace,    "Space parity" }
    };
    auto converted = mapToString.find( parity );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "Unknown parity";
} // ParityToString

} // namespace connections

} // namespace logika

/// @file Объявление функций для проверки типов соединений
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_UTILS_TYPES_CHECKER_INL
#define LOGIKA_CONNECTIONS_UTILS_TYPES_CHECKER_INL

#include <logika/connections/utils/types_checker.hpp>

namespace logika
{

namespace connections
{

inline bool IsValidConnectionType( ConnectionType::Type type )
{
    return type == ConnectionType::Offline
        || type == ConnectionType::Serial
        || type == ConnectionType::Network
        || type == ConnectionType::ComPort
        || type == ConnectionType::Tcp
        || type == ConnectionType::Udp
        || type == ConnectionType::Modem
        || type == ConnectionType::Radius;
} // IsValidConnectionType


inline bool IsValidPurgeFlags( PurgeFlags::Type flags )
{
    return flags == PurgeFlags::Rx
        || flags == PurgeFlags::Tx
        || flags == PurgeFlags::TxRx;
} // IsValidPurgeFlags


inline bool IsValidBaudRate( BaudRate::Type rate )
{
    return     rate != BaudRate::NotSupported
        &&  (  rate == BaudRate::Rate110
            || rate == BaudRate::Rate300
            || rate == BaudRate::Rate600
            || rate == BaudRate::Rate1200
            || rate == BaudRate::Rate2400
            || rate == BaudRate::Rate4800
            || rate == BaudRate::Rate9600
            || rate == BaudRate::Rate19200
            || rate == BaudRate::Rate38400
            || rate == BaudRate::Rate57600
            || rate == BaudRate::Rate115200 );
} // IsValidBaudRate


inline bool IsValidStopBits( StopBits::Type bits )
{
    return     bits != StopBits::NotSupported
        &&  (  bits == StopBits::StopBitOne
            || bits == StopBits::StopBitOneAndHalf
            || bits == StopBits::StopBitsTwo );
} // IsValidStopBits


inline bool IsValidDataBits( DataBits::Type bits )
{
    return     bits != DataBits::NotSupported
        &&  (  bits == DataBits::DataBits5
            || bits == DataBits::DataBits6
            || bits == DataBits::DataBits7
            || bits == DataBits::DataBits8 );
} // IsValidDataBits


inline bool IsValidParity( Parity::Type parity )
{
    return     parity != Parity::NotSupported
        &&  (  parity == Parity::ParityNone
            || parity == Parity::ParityEven
            || parity == Parity::ParityOdd
            || parity == Parity::ParityMark
            || parity == Parity::ParitySpace );
} // IsValidParity

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_UTILS_TYPES_CHECKER_INL

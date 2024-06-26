/// @file Реализация базового класса для работы с соединением по сети
/// @copyright HypeRRu 2024

#include <logika/connections/network/net_connection.h>

/// @cond
#include <iostream>
/// @endcond

namespace logika
{

namespace connections
{

NetConnection::NetConnection( const std::string& serverHostName, uint16_t serverPort, TimeType readTimeout )
    : Connection( serverHostName + ":" + std::to_string( serverPort ), readTimeout )
    , serverHostName_{ serverHostName }
    , serverPort_{ serverPort }
{
    type_ = ConnectionType::Network;
} // NetConnection

} // namespace connections

} // namespace logika

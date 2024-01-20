/// @file Реализация базового класса для работы с соединением по сети
/// @copyright HypeRRu 2024

#include <logika/connections/network/net_connection.h>

#include <iostream>

namespace logika
{

namespace connections
{

NetConnection::NetConnection( const std::string& serverHostName, uint16_t serverPort, uint32_t readTimeout )
    : Connection( serverHostName + ":" + std::to_string( serverPort ) )
    , serverHostName_{ serverHostName }
    , serverPort_{ serverPort }
{
    type_ = ConnectionType::Network;
} // NetConnection

} // namespace connections

} // namespace logika

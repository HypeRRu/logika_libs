/// @file Реализация класса для работы с соединением по TCP
/// @copyright HypeRRu 2024

#include <logika/connections/network/tcp_connection.h>

#include <logika/log/defines.h>

namespace logika
{

namespace connections
{

TcpConnection::TcpConnection( const std::string& serverHostName, uint16_t serverPort, uint32_t readTimeout )
    : NetConnection( serverHostName, serverPort, readTimeout )
    , socket_{ LOGIKA_SOCKET_INVALID }
{
    type_ = ConnectionType::Tcp;
} // TcpConnection


TcpConnection::~TcpConnection()
{
    Close();
} // ~TcpConnection

} // namespace connections

} // namespace logika

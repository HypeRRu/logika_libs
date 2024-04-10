/// @file Реализация класса для работы с соединением по UDP
/// @copyright HypeRRu 2024

#include <logika/connections/network/udp_connection.h>

#include <logika/log/defines.h>

namespace logika
{

namespace connections
{

UdpConnection::UdpConnection( const std::string& serverHostName, uint16_t serverPort, uint32_t readTimeout )
    : NetConnection( serverHostName, serverPort, readTimeout )
    , socket_{ LOGIKA_SOCKET_INVALID }
{
    if ( 0 == readTimeout_ )
    {
        readTimeout_ = 1000;
        LOG_WRITE( LOG_WARNING, LOCALIZED( "UDP connection did not work properly without read timeout. " )
                                LOCALIZED( "Setting read timeout to " ) << readTimeout_ << LOCALIZED( " ms" ) );
    }
    type_ = ConnectionType::Udp;
} // UdpConnection


UdpConnection::~UdpConnection()
{
    Close();
} // ~UdpConnection

} // namespace connections

} // namespace logika

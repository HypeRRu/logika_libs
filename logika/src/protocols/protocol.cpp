/// @file Реализация базового класса для работы с протоколами
/// @copyright HypeRRu 2024

#include <logika/protocols/protocol.h>

#include <logika/connections/iconnection.h>
#include <logika/log/defines.h>

namespace logika
{

namespace protocols
{

Protocol::Protocol()
    : connection_{ nullptr }
    , packetsSent_{ 0 }
    , packetsReceived_{ 0 }
    , rxTimeout_{ 0 }
    , rxCrc_{ 0 }
    , rxLatePacket_{ 0 }
    , rxGeneral_{ 0 }
{} // Protocol


std::shared_ptr< connections::IConnection > Protocol::GetConnection() const
{
    return connection_;
} // GetConnection


void Protocol::SetConnection( std::shared_ptr< connections::IConnection > connection )
{
    LOG_WRITE( LOG_INFO, ( connection ? L"Set" : L"Reset" ) << L" protocol connection" );
    connection_ = connection;
} // SetConnection


uint32_t Protocol::GetPacketsSent() const
{
    return packetsSent_;
} // GetPacketsSent


uint32_t Protocol::GetPacketsReceived() const
{
    return packetsReceived_;
} // GetPacketsReceived


uint32_t Protocol::GetRxTimeouts() const
{
    return rxTimeout_;
} // GetRxTimeouts


uint32_t Protocol::GetRxCrc() const
{
    return rxCrc_;
} // GetRxCrc


uint32_t Protocol::GetRxLatePackets() const
{
    return rxLatePacket_;
} // GetRxLatePackets


uint32_t Protocol::GetRxGeneralErrors() const
{
    return rxGeneral_;
} // GetRxGeneralErrors


uint32_t Protocol::GetGeneralErrors() const
{
    return generalError_;
} // GetGeneralErrors


void Protocol::RegisterEvent( Rc::Type event )
{
    switch ( event )
    {
        case Rc::Success:
            return;
        case Rc::PacketReceived:
            LOG_WRITE_MSG( LOG_DEBUG, L"Packet received event registered" );
            ++packetsReceived_;
            break;
        case Rc::PacketTransmitted:
            LOG_WRITE_MSG( LOG_DEBUG, L"Packet transmitted event registered" );
            ++packetsSent_;
            break;
        case Rc::RxTimeoutError:
            LOG_WRITE_MSG( LOG_DEBUG, L"Read Timeout error registered" );
            ++rxTimeout_;
            break;
        case Rc::RxCrcError:
            LOG_WRITE_MSG( LOG_DEBUG, L"Read CRC error registered" );
            ++rxCrc_;
            break;
        case Rc::RxLatePacketError:
            LOG_WRITE_MSG( LOG_DEBUG, L"Read Packet Late error registered" );
            ++rxLatePacket_;
            break;
        case Rc::RxGeneralError:
            LOG_WRITE_MSG( LOG_DEBUG, L"Read general error registered" );
            ++rxLatePacket_;
            break;
        default:
            LOG_WRITE( LOG_DEBUG, L"General error registered. Code " << event );
            ++generalError_;
            break;
    }
    /// @todo monitor event
} // RegisterEvent


void Protocol::Reset()
{
    packetsSent_ = 0;
    packetsReceived_ = 0;
    rxTimeout_ = 0;
    rxCrc_ = 0;
    rxLatePacket_ = 0;
    rxGeneral_ = 0;
    generalError_ = 0;

    ResetImpl();
} // Reset


void Protocol::ResetImpl()
{
} // ResetImpl

} // namespace protocols

} // namespace logika

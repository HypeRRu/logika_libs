/// @file Реализация базового класса для работы с протоколами
/// @copyright HypeRRu 2024

#include <logika/protocols/protocol.h>

#include <logika/connections/iconnection.h>
#include <logika/log/defines.h>

#include <logika/protocols/m4/m4protocol.h>
#include <logika/protocols/x6/spbusprotocol.h>

#include <logika/meters/logika4/logika4.h>

/// @cond
#include <chrono>
/// @endcond

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
    , waitMtx_{}
    , waitCond_{}
{} // Protocol


std::shared_ptr< connections::IConnection > Protocol::GetConnection() const
{
    return connection_;
} // GetConnection


void Protocol::SetConnection( std::shared_ptr< connections::IConnection > connection )
{
    LOG_WRITE( LOG_INFO, ( connection ? LOCALIZED( "Set" ) : LOCALIZED( "Reset" ) ) << LOCALIZED( " protocol connection" ) );
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
            LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Packet received event registered" ) );
            ++packetsReceived_;
            break;
        case Rc::PacketTransmitted:
            LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Packet transmitted event registered" ) );
            ++packetsSent_;
            break;
        case Rc::RxTimeoutError:
            LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Read Timeout error registered" ) );
            ++rxTimeout_;
            break;
        case Rc::RxCrcError:
            LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Read CRC error registered" ) );
            ++rxCrc_;
            break;
        case Rc::RxLatePacketError:
            LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Read Packet Late error registered" ) );
            ++rxLatePacket_;
            break;
        case Rc::RxGeneralError:
            LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Read general error registered" ) );
            ++rxLatePacket_;
            break;
        default:
            LOG_WRITE( LOG_DEBUG, LOCALIZED( "General error registered. Code " ) << event );
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

    ResetBusActiveState();
} // Reset


void Protocol::CloseCommSession( ByteType srcNt, ByteType dstNt )
{
    if ( connection_
        && connection_->GetConnectionState() == connections::ConnectionState::Connected )
    {
        CloseCommSessionImpl( srcNt, dstNt );
    }
} // CloseCommSession


void Protocol::ResetBusActiveState()
{} // ResetBusActiveState


void Protocol::CloseCommSessionImpl( ByteType srcNt, ByteType dstNt )
{
    (void) srcNt;
    (void) dstNt;
} // CloseCommSessionImpl


bool Protocol::WaitFor( TimeType duration )
{
    std::unique_lock< std::mutex > waitLock{ waitMtx_ };
    return waitCond_.wait_for( waitLock, std::chrono::milliseconds( duration ) )
        == std::cv_status::timeout;
} // WaitFor


void Protocol::CancelWait()
{
    std::unique_lock< std::mutex > waitLock{ waitMtx_ };
    waitCond_.notify_all();
} // CancelWait


std::shared_ptr< meters::Meter > Protocol::DetectM4( std::shared_ptr< M4::M4Protocol > bus,
    const storage::StorageKeeper& sKeeper, ByteVector& dump, LocString& model )
{
    /// @todo Реализовать
    model = LOCALIZED( "" );

    auto meterStorage = sKeeper.GetStorage< LocString, meters::Meter >();
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< meters::Meter > spt942 = nullptr;
    meterStorage->GetItem( LOCALIZED( "SPT942" ), spt942 );

    // M4::M4Packet reply = bus->Handshake( static_cast< ByteType >( 0xFF ), 0, false );
    // dump = reply.GetDump();
    // if ( reply.data.size() < 3 )
    // {
    //     return nullptr;
    // }
    // std::shared_ptr< meters::Meter > meter = Logika4::DetermineMeter(
    //     reply.data[ 0 ], reply.data[ 1 ], reply.data[ 2 ], meterStorage );
    // if ( !meter )
    // {
    //     return nullptr;
    // }

    // if ( meter == spt942 )
    // {
    //     std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >( meter );
    //     const ByteVector modelBytes = bus.ReadFlashBytes(
    //           meter4l
    //         , static_cast< ByteType >( 0xFF )
    //         , static_cast< ByteType >( 0x30 )
    //         , 1
    //     );
    //     if ( !modelBytes.empty() )
    //     {
    //         model = LocString( static_cast< LocChar >( modelBytes.at( 0 ) ), 1 );
    //     }
    // }
    
    // return meter;

    return nullptr;
} // DetectM4
    

std::shared_ptr< meters::Meter > Protocol::DetectX6( std::shared_ptr< X6::SPBusProtocol > bus,
    const storage::StorageKeeper& sKeeper, ByteVector& dump, LocString& model )
{
    throw std::runtime_error{ "Not implemented" };
} // DetectX6


std::shared_ptr< meters::Meter > Protocol::DetectResponse( std::shared_ptr< connections::IConnection > connection,
    const storage::StorageKeeper& sKeeper, ByteVector& dump, LocString& model, bool& rxDetected )
{
    /// @todo реализовать
    return nullptr;
} // DetectResponse


std::shared_ptr< meters::Meter > Protocol::AutodectSpt( std::shared_ptr< connections::IConnection > connection,
    const storage::StorageKeeper& sKeeper, connections::BaudRate::Type fixedBaudRate,
    uint32_t waitTimeout, bool tryM4, bool trySpBus, bool tryMek,
    ByteType srcAddr, ByteType dstAddr, ByteVector& dump,
    connections::BaudRate::Type& deviceBaudRate, LocString& model )
{
    /// @todo реализовать
    return nullptr;
} // AutodectSpt


uint32_t Protocol::GetDefaultTimeout( meters::BusProtocolType proto, connections::ConnectionType::Type connType )
{
    switch ( connType )
    {
        case connections::ConnectionType::Offline:
        case connections::ConnectionType::Serial:
            if ( proto == meters::BusProtocolType::SPBus )
            {
                return 15000;
            }
            return 5000;
        case connections::ConnectionType::Modem:
            if ( proto == meters::BusProtocolType::SPBus )
            {
                return 15000;
            }
            return 10000;
        case connections::ConnectionType::Udp:
            return 10000;
        case connections::ConnectionType::Tcp:
        case connections::ConnectionType::Radius:
        default:
            return 15000;
    }
} // GetDefaultTimeout


uint16_t Protocol::Crc16( uint16_t crc, const ByteVector& buffer, uint32_t offset, uint32_t length )
{
    if ( offset >= buffer.size() || buffer.size() - offset < length )
    {
        throw std::runtime_error{ "Incorrect length and offset" };
    }

    uint16_t cSum = crc;
    for ( uint32_t current = offset; current < offset + length; ++current )
    {
        cSum ^= static_cast< uint16_t >( buffer.at( current ) << 8 );
        for ( uint32_t iteration = 0; iteration < 8; ++iteration )
        {
            if ( ( cSum & 0x8000 ) != 0 )
            {
                cSum = static_cast< uint16_t >( ( cSum << 1 ) ^ 0x1021 );
            }
            else
            {
                cSum <<= 1;
            }
        }
    }
    return cSum;
} // Crc16

} // namespace protocols

} // namespace logika

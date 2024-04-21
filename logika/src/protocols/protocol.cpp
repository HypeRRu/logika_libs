/// @file Реализация базового класса для работы с протоколами
/// @copyright HypeRRu 2024

#include <logika/protocols/protocol.h>

#include <logika/connections/iconnection.h>
#include <logika/connections/serial/serial_connection.h>
#include <logika/connections/utils/types_converter.h>
#include <logika/log/defines.h>

#include <logika/protocols/m4/m4protocol.h>
#include <logika/protocols/x6/spbusprotocol.h>
#include <logika/protocols/m4/packet.h>

#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/meters/data_tag.h>
#include <logika/meters/interval_archive.h>

/// @cond
#include <chrono>
#include <array>
/// @endcond

namespace logika
{

namespace protocols
{

Protocol::Protocol( const storage::StorageKeeper& sKeeper )
    : connection_{ nullptr }
    , packetsSent_{ 0 }
    , packetsReceived_{ 0 }
    , rxTimeout_{ 0 }
    , rxCrc_{ 0 }
    , rxLatePacket_{ 0 }
    , rxGeneral_{ 0 }
    , waitMtx_{}
    , waitCond_{}
    , sKeeper_{ sKeeper }
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


void Protocol::CloseCommSession( const ByteType* srcNt, const ByteType* dstNt )
{
    if ( connection_
        && connection_->GetConnectionState() == connections::ConnectionState::Connected )
    {
        CloseCommSessionImpl( srcNt, dstNt );
    }
} // CloseCommSession


void Protocol::ResetBusActiveState()
{} // ResetBusActiveState


void Protocol::CloseCommSessionImpl( const ByteType* srcNt, const ByteType* dstNt )
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


std::shared_ptr< meters::Meter > Protocol::GetMeterType( const ByteType* srcNt,
    const ByteType* dstNt, LocString& extraData )
{
    (void) srcNt;
    (void) dstNt;
    (void) extraData;
    return nullptr;
} // GetMeterType


void Protocol::UpdateTags( const ByteType* srcNt, const ByteType* dstNt,
    std::vector< std::shared_ptr< meters::DataTag > >& tags )
{
    (void) srcNt;
    (void) dstNt;
    (void) tags;
} // UpdateTags


std::shared_ptr< meters::IntervalArchive > Protocol::ReadIntervalArchiveDef( std::shared_ptr< meters::Meter > meter,
    const ByteType* srcNt, const ByteType* dstNt, std::shared_ptr< meters::ArchiveType > archiveType,
    std::shared_ptr< logika::Any >& state )
{
    (void) meter;
    (void) srcNt;
    (void) dstNt;
    (void) archiveType;
    (void) state;
    return nullptr;
} // ReadIntervalArchiveDef


bool Protocol::ReadIntervalArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
    const ByteType* dstNt, std::shared_ptr< meters::IntervalArchive > archive, TimeType start, TimeType end,
    std::shared_ptr< logika::Any >& state, double& progress )
{
    (void) meter;
    (void) srcNt;
    (void) dstNt;
    (void) archive;
    (void) start;
    (void) end;
    (void) state;
    (void) progress;
    return false;
} // ReadIntervalArchive


bool Protocol::ReadServiceArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
    const ByteType* dstNt, std::shared_ptr< meters::ServiceArchive > archive, TimeType start, TimeType end,
    std::shared_ptr< logika::Any >& state, double& progress )
{
    (void) meter;
    (void) srcNt;
    (void) dstNt;
    (void) archive;
    (void) start;
    (void) end;
    (void) state;
    (void) progress;
    return false;
} // ReadServiceArchive


TimeType Protocol::GetDeviceClock( std::shared_ptr< meters::Meter > meter,
    const ByteType* srcNt, const ByteType* dstNt )
{
    (void) meter;
    (void) srcNt;
    (void) dstNt;
    return 0;
} // GetDeviceClock


std::shared_ptr< meters::Meter > Protocol::DetectM4( std::shared_ptr< M4::M4Protocol > bus,
    const storage::StorageKeeper& sKeeper, ByteVector& dump, LocString& model )
{
    model = LOCALIZED( "" );

    auto meterStorage = sKeeper.GetStorage< LocString, meters::Meter >();
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< meters::Meter > spt942 = nullptr;
    meterStorage->GetItem( LOCALIZED( "SPT942" ), spt942 );

    ByteType nt = M4::M4Protocol::BROADCAST_NT;
    M4::Packet reply = bus->DoHandshake( &nt, 0, false );
    dump = reply.GetDump();
    if ( reply.data.size() < 3 )
    {
        return nullptr;
    }
    std::shared_ptr< meters::Meter > meter = meters::Logika4::DetermineMeter(
        reply.data[ 0 ], reply.data[ 1 ], reply.data[ 2 ], meterStorage );
    if ( !meter )
    {
        return nullptr;
    }

    if ( meter == spt942 )
    {
        std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >( meter );
        ByteType dstNt = static_cast< ByteType >( M4::M4Protocol::BROADCAST_NT );
        const ByteVector modelBytes = bus->ReadFlashBytesL4(
              meter4l
            , &dstNt
            , static_cast< ByteType >( 0x30 )
            , 1
        );
        if ( !modelBytes.empty() )
        {
            model = LocString( static_cast< LocChar >( modelBytes.at( 0 ) ), 1 );
        }
    }
    
    return meter;
} // DetectM4
    

std::shared_ptr< meters::Meter > Protocol::DetectX6( std::shared_ptr< X6::SPBusProtocol > bus,
    const storage::StorageKeeper& sKeeper, ByteVector& dump, LocString& model )
{
    throw std::runtime_error{ "Not implemented" };
} // DetectX6


std::shared_ptr< meters::Meter > Protocol::AutodectSpt( std::shared_ptr< connections::IConnection > connection,
    const storage::StorageKeeper& sKeeper, connections::BaudRate::Type fixedBaudRate,
    uint32_t waitTimeout, bool tryM4, bool trySpBus, bool tryMek,
    const ByteType* srcAddr, const ByteType* dstAddr, ByteVector& dump,
    connections::BaudRate::Type& deviceBaudRate, LocString& model )
{
    if ( !connection )
    {
        LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Connection not set" ) );
        return nullptr;
    }

    std::shared_ptr< meters::Meter > meter = nullptr;
    model = LOCALIZED( "" );

    std::shared_ptr< M4::M4Protocol > bus4 = std::make_shared< M4::M4Protocol >( sKeeper );
    bus4->SetConnection( connection );
    std::shared_ptr< X6::SPBusProtocol > bus6 = std::make_shared< X6::SPBusProtocol >( sKeeper, true );
    bus6->SetConnection( connection );

    constexpr std::array< connections::BaudRate::Type, 7 > baudRatesAvailable{
          connections::BaudRate::Rate2400, connections::BaudRate::Rate57600
        , connections::BaudRate::Rate4800, connections::BaudRate::Rate19200
        , connections::BaudRate::Rate9600, connections::BaudRate::Rate38400
        , connections::BaudRate::Rate115200
    };
    bool canChangeBaudRate = ( connection->GetConnectionType() & connections::ConnectionType::Serial );
    connections::BaudRate::Type detectedBaudRate = connections::BaudRate::NotSupported;
    TimeType savedTimeout = connection->GetReadTimeout();
    connection->SetReadTimeout( 500 );

    try
    {
        std::vector< connections::BaudRate::Type > baudRates{ fixedBaudRate };
        if ( canChangeBaudRate && fixedBaudRate == connections::BaudRate::NotSupported )
        {
            baudRates = std::vector< connections::BaudRate::Type >(
                baudRatesAvailable.cbegin(), baudRatesAvailable.cend() );
        }

        for ( connections::BaudRate::Type br: baudRates )
        {
            if ( canChangeBaudRate )
            {
                std::shared_ptr< connections::SerialConnection > serialConnection
                    = std::dynamic_pointer_cast< connections::SerialConnection >( connection );
                if ( !serialConnection )
                {
                    throw std::runtime_error{ "Invalid connection type: need to be serial" };
                }
                detectedBaudRate = br;
                serialConnection->SetBaudRate( br );
                serialConnection->SetDataBits( connections::DataBits::DataBits8 );
                serialConnection->SetStopBits( connections::StopBits::StopBitOne );
                serialConnection->SetParity(   connections::Parity::ParityNone );
                LOG_WRITE( LOG_DEBUG, LOCALIZED( "Trying speed " )
                    << ToLocString( connections::BaudRateToString( br ) ) );
            }

            if ( tryM4 )
            {
                try
                {
                    meter = DetectM4( bus4, sKeeper, dump, model );
                    deviceBaudRate = detectedBaudRate;
                    return meter;
                }
                catch ( ... )
                {}
            }

            if ( trySpBus )
            {
                try
                {
                    meter = DetectX6( bus6, sKeeper, dump, model );
                    deviceBaudRate = detectedBaudRate;
                    return meter;
                }
                catch ( ... )
                {}
            }
        }

        if ( tryMek && trySpBus && canChangeBaudRate )
        {
            /// @note not implemented
        }
    }
    catch ( ... )
    {}

    LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Device not recognized" ) );
    connection->SetReadTimeout( savedTimeout );
    deviceBaudRate = connections::BaudRate::NotSupported;
    dump = ByteVector{};
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

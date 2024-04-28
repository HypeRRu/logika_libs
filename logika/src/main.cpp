#include <logika/connections/serial/serial_port_connection.h>
#include <logika/connections/network/udp_connection.h>
#include <logika/connections/network/tcp_connection.h>
#include <logika/log/logger.h>
#include <logika/log/defines.h>

#include <logika/meters/types.h>
#include <logika/protocols/m4/packet.h>

#include <logika/common/any.hpp>
#include <logika/meters/archive_field_def.h>

#include <logika/resources/loader.hpp>
#include <logika/resources/l4_archive_fields.pb.h>

#include <logika/storage/storage.hpp>
#include <logika/meters/converters/archive_type_converter.h>
#include <logika/meters/converters/device_converter.h>
#include <logika/meters/converters/channel_converter.h>
#include <logika/meters/converters/m4_tag_converter.h>
#include <logika/meters/converters/m4_archive_converter.h>
#include <logika/meters/converters/m4_archive_field_converter.h>
#include <logika/meters/converters/l4_tag_converter.h>
#include <logika/meters/converters/l4_archive_converter.h>
#include <logika/meters/converters/l4_archive_field_converter.h>

#include <logika/meters/data_table.hpp>
#include <logika/meters/interval_archive.h>

#include <logika/meters/meter.h>
#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/meters/data_tag.h>

#include <logika/meters/service_archive.h>
#include <logika/meters/interval_archive.h>
#include <logika/protocols/m4/m4protocol.h>
#include <logika/protocols/m4/tag_write_data.h>

#include <logika/common/misc.h>

/// @cond
#include <iostream>

#if defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#endif
/// @endcond

void LoadResources( const std::string& pathTo, logika::storage::StorageKeeper& sKeeper, bool silent = true );

int main()
{
#if defined( LOGIKA_USE_LOGGER )
    logika::log::Logger& logger = logika::log::Logger::Instance();
    logger.SetLogType( logika::log::LogType::LogConsole );
    logger.SetLogLevel( logika::log::LogLevel::LogAll );
#endif

#if defined( _WIN32 ) || defined( _WIN64 )
    WSADATA wsaData;
    int wsaInitResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if ( 0 != wsaInitResult) {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "WSAStartup failed: " ) << wsaInitResult );
        return 1;
    }
#endif

#if 0
    logika::ByteVector buffer{ 'b', 'u', 'f', 'f', 'e', 'r', '\n', '\0' };
#if defined( __linux__ ) || defined( __APPLE__ )
    logika::connections::SerialPortConnection con{ "/tmp/ttyV0", 1000 };
#endif
#if defined( _WIN32 ) || defined( _WIN64 )
    logika::connections::SerialPortConnection con{ "\\\\.\\COM10", 1000 };
#endif
    con.SetBaudRate( logika::connections::BaudRate::Rate110 );
    con.SetStopBits( logika::connections::StopBits::StopBitsTwo );
    con.SetDataBits( logika::connections::DataBits::DataBits5 );
    con.SetParity( logika::connections::Parity::ParityEven );
    con.Open();
    con.Write( buffer );
    con.Purge( logika::connections::PurgeFlags::TxRx );
    logika::ByteVector rdbuf;
    con.Read( rdbuf, 0, 5 );

    std::string comRd{ rdbuf.data(), rdbuf.size() };
    LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "" ) );
    LOG_WRITE( LOG_INFO, LOCALIZED( "Read from " ) << logika::ToLocString( con.GetAddress() )
        << LOCALIZED( ": " ) << comRd );
    LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "" ) );

    logika::connections::UdpConnection udpCon{ "127.0.0.1", 8083, 30000 };
    udpCon.Open();
    udpCon.Write( buffer );
    logika::ByteVector udpRdbuf;
    udpCon.Read( udpRdbuf, 0, 6 );
    udpCon.Purge( logika::connections::PurgeFlags::TxRx );
    udpCon.Read( udpRdbuf, 0, 5 );

    std::string udpRd{ udpRdbuf.data(), udpRdbuf.size() };
    LOG_WRITE( LOG_INFO, LOCALIZED( "" ) );
    LOG_WRITE( LOG_INFO, LOCALIZED( "Read from " ) << logika::ToLocString( udpCon.GetAddress() ) << LOCALIZED( ": " ) << udpRd );
    LOG_WRITE( LOG_INFO, LOCALIZED( "" ) );

    logika::connections::TcpConnection tcpCon{ "127.0.0.1", 8084, 30000 };
    tcpCon.Open();
    tcpCon.Write( buffer );
    logika::ByteVector tcpRdbuf;
    tcpCon.Read( tcpRdbuf, 0, 6 );
    tcpCon.Purge( logika::connections::PurgeFlags::TxRx );
    tcpCon.Read( tcpRdbuf, 0, 5 );

    std::string tcpRd{ tcpRdbuf.data(), tcpRdbuf.size() };
    LOG_WRITE( LOG_INFO, LOCALIZED( "" ) );
    LOG_WRITE( LOG_INFO, LOCALIZED( "Read from " ) << logika::ToLocString( tcpCon.GetAddress() )
        << LOCALIZED( ": " ) << tcpRd );
    LOG_WRITE( LOG_INFO, LOCALIZED( "" ) );
#endif // if 0

    logika::meters::VitalInfo vi{ LOCALIZED( "0x11" ), LOCALIZED( "rev" ), LOCALIZED( "serial" )
        , { LOCALIZED( "eth0" ), LOCALIZED( "eth1" ) }
        , static_cast< logika::ByteType >( 0xFF ), static_cast< logika::ByteType >( 0xFF )
        , static_cast< logika::ByteType >( 0xFF ), LOCALIZED( "crc16" ) };
    logika::meters::HistoricalSeries hs{ 0x11, { { nullptr, 0, 0 } } };
    logika::meters::VQT vqt{};

#if 1
    logika::storage::StorageKeeper sKeeper = logika::storage::StorageKeeper::Instance();
    LoadResources( "/home/hyper/bin_data/", sKeeper, false );
    auto atStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveType >();
    auto mStorage = sKeeper.GetStorage< logika::LocString, logika::meters::Meter >();

    // auto cdKeys = cdStorage->GetKeys();
    // for ( auto key: cdKeys )
    // {
    //     LOG_WRITE( LOG_INFO, LOCALIZED( "ChannelDef key '" ) << key << LOCALIZED( "'" ) );
    // }

    auto mKeys = mStorage->GetKeys();
    for ( auto key: mKeys )
    {
        std::shared_ptr< logika::meters::Meter > meterItem;
        mStorage->GetItem( key, meterItem );
        if ( meterItem )
        {
            meterItem->Init( sKeeper );
        }
    }

    logika::meters::ArchiveFieldDefSettings afdSettings1;
    afdSettings1.ordinal = 10;
    afdSettings1.name = LOCALIZED( "tag1" );
    atStorage->GetItem( LOCALIZED( "Hour" ), afdSettings1.archiveType );

    logika::meters::ChannelDef cdef1{ nullptr, LOCALIZED( "chn" ), 0, 10, LOCALIZED( "some channel" ) };
    std::shared_ptr< logika::meters::ArchiveFieldDef > afd1 =
        std::make_shared< logika::meters::ArchiveFieldDef >( cdef1, afdSettings1 );

    logika::meters::ArchiveFieldDefSettings afdSettings2;
    afdSettings2.ordinal = 1;
    afdSettings2.name = LOCALIZED( "tag2" );
    atStorage->GetItem( LOCALIZED( "Hour" ), afdSettings2.archiveType );

    logika::meters::ChannelDef cdef2{ nullptr, LOCALIZED( "chn1" ), 0, 10, LOCALIZED( "some channel 2" ) };
    std::shared_ptr< logika::meters::ArchiveFieldDef > afd2 =
        std::make_shared< logika::meters::ArchiveFieldDef >( cdef2, afdSettings2 );

    logika::meters::ArchiveFieldDefSettings afdSettings3;
    afdSettings3.ordinal = 2;
    afdSettings3.name = LOCALIZED( "TM" );
    atStorage->GetItem( LOCALIZED( "Hour" ), afdSettings3.archiveType );

    logika::meters::ChannelDef cdef3{ nullptr, LOCALIZED( "tmChn" ), 0, 10, LOCALIZED( "timestamp channel" ) };
    std::shared_ptr< logika::meters::ArchiveFieldDef > afd3 =
        std::make_shared< logika::meters::ArchiveFieldDef >( cdef3, afdSettings3 );

    std::vector< logika::meters::DataTable::FieldType > fields;
    fields.push_back(
        std::make_shared< logika::meters::ArchiveField >( afd1, 8 )
    );
    fields.push_back(
        std::make_shared< logika::meters::ArchiveField >( afd3, 7 )
    );
    fields.push_back(
        std::make_shared< logika::meters::ArchiveField >( afd2, 5 )
    );
    std::shared_ptr< logika::meters::DataTable > table = std::make_shared< logika::meters::DataTable >( fields );

    std::shared_ptr< logika::meters::ArchiveType > monthArchive;
    std::shared_ptr< logika::meters::Meter > lgk410meter;
    atStorage->GetItem( logika::LocString{ LOCALIZED( "Month" ) }, monthArchive );
    if ( lgk410meter )
    {
        lgk410meter->Init( sKeeper );
    }
    logika::meters::IntervalArchive intervalArchive{
        lgk410meter,
        monthArchive,
        table
    };

    auto dt = intervalArchive.GetDataTable();
    for ( auto field: dt->GetFieldsList() )
    {
        if ( field )
        {
            LOG_WRITE( LOG_INFO, LOCALIZED( "DataTable column '" ) << field->GetName() << LOCALIZED( "'" ) );
        }
    }

    auto meter = logika::meters::Meter::Create< logika::meters::Logika4L >(
        logika::meters::MeasureKind::Undefined,
        LOCALIZED( "DEV0" ),
        LOCALIZED( "Some device" ),
        1,
        1,
        logika::meters::BusProtocolType::RSBus
    );
    if ( !meter )
    {
        LOG_WRITE( LOG_INFO, LOCALIZED( "Unable to create Logika4L" ) );
    }
    else
    {
        LOG_WRITE( LOG_INFO, LOCALIZED( "Logika4L created" ) );
        LOG_WRITE( LOG_INFO, meter->GetCaption() << LOCALIZED( "(" ) << meter->GetDescription() << LOCALIZED( ")" ) );
    }
#endif // if 0
    
    const std::string ipAddress{ "91.209.59.238" };
    const uint16_t addressPort = 8002;
    std::shared_ptr< logika::protocols::M4::M4Protocol > bus4
        = std::make_shared< logika::protocols::M4::M4Protocol >( sKeeper );
    std::shared_ptr< logika::connections::TcpConnection > tcpBusConn
        = std::make_shared< logika::connections::TcpConnection >( ipAddress, addressPort, 20000 );
    tcpBusConn->Open();
    bus4->SetConnection( tcpBusConn );

    // try
    {
        logika::ByteVector meterDump{};
        logika::LocString meterModel{};
        std::shared_ptr< logika::meters::Meter > detectedMeter = logika::protocols::Protocol::DetectM4(
            bus4, sKeeper, meterDump, meterModel );
        if ( !detectedMeter )
        {
            LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Meter detection failed" ) );
        }
        else
        {
            LOG_WRITE( LOG_INFO, LOCALIZED( "Detected meter is " ) << detectedMeter->GetCaption() );
        }

        if ( detectedMeter )
        {
            try
            {
                std::shared_ptr< logika::Any > state = nullptr;
                std::shared_ptr< logika::meters::ArchiveType > hourArchiveType = nullptr;
                atStorage->GetItem( LOCALIZED( "Hour" ), hourArchiveType );
                std::shared_ptr< logika::meters::IntervalArchive > intArchive
                    = bus4->ReadIntervalArchiveDef( detectedMeter, nullptr, nullptr, hourArchiveType, state );

                if ( intArchive )
                {
                    LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Reading interval archive" ) );
                    double progress = 0.0;
                    while ( bus4->ReadIntervalArchive( detectedMeter, nullptr, nullptr, intArchive,
                        logika::GetCurrentTimestamp() - 1000ull * 60 * 60 * 24 * 5,
                        logika::GetCurrentTimestamp(), state, progress ) )
                    {
                        LOG_WRITE( LOG_INFO, LOCALIZED( "Read interval archive progress is " ) << progress << LOCALIZED( "%" ) );
                    }
                    LOG_WRITE( LOG_INFO, LOCALIZED( "After read interval archive progress is " ) << progress << LOCALIZED( "%" ) );
                }
                else
                {
                    LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Unable to read interval archive def" ) );
                }
            }
            catch( const std::exception& e )
            {
                LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
            }
        }
        if ( detectedMeter )
        {
            try
            {
                std::shared_ptr< logika::meters::ArchiveType > errorsArchiveType = nullptr;
                atStorage->GetItem( LOCALIZED( "ErrorsLog" ), errorsArchiveType );
                std::shared_ptr< logika::meters::ServiceArchive > servArchive
                    = std::make_shared< logika::meters::ServiceArchive >( detectedMeter, errorsArchiveType );

                std::shared_ptr< logika::Any > state
                    = std::make_shared< logika::Any >( static_cast< logika::TimeType >( 0 ) );
                double progress = 0.0;

                while ( bus4->ReadServiceArchive( detectedMeter, nullptr, nullptr, servArchive,
                    logika::GetCurrentTimestamp() - 1000ull * 60 * 60 * 24 * 1,
                    logika::GetCurrentTimestamp(), state, progress ) )
                {
                    LOG_WRITE( LOG_INFO, LOCALIZED( "Read service archive progress is " ) << progress << LOCALIZED( "%" ) );
                }
                LOG_WRITE( LOG_INFO, LOCALIZED( "After read service archive progress is " ) << progress << LOCALIZED( "%" ) );
            }
            catch( const std::exception& e )
            {
                LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
            }
        }
        if ( detectedMeter )
        {
            try
            {
                logika::TimeType devClock = bus4->GetDeviceClock( detectedMeter, nullptr, nullptr );
                LOG_WRITE( LOG_INFO, LOCALIZED( "Device time: " ) << logika::GetTimeString( devClock ) );
            }
            catch( const std::exception& e )
            {
                LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
            }
        }
        if ( detectedMeter )
        {
            try
            {
                std::shared_ptr< logika::meters::Logika4L > dm4l
                    = std::dynamic_pointer_cast< logika::meters::Logika4L >( detectedMeter );
                if ( dm4l )
                {
                    LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Detected meter is Logika4L" ) );
                    LOG_WRITE( LOG_INFO, LOCALIZED( "Write Logika4L parameter 0 RC: " )
                        << bus4->WriteParameterL4( dm4l, nullptr, 0, 0, LOCALIZED( "Test value" ), false ) );
                }
            }
            catch( const std::exception& e )
            {
                LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
            }
        }
        // if ( detectedMeter )
        // {
        //     try
        //     {
        //         std::shared_ptr< logika::meters::Logika4M > dm4m
        //             = std::dynamic_pointer_cast< logika::meters::Logika4M >( detectedMeter );
        //         if ( dm4m )
        //         {
        //             LOG_WRITE_MSG( LOG_INFO, LOCALIZED( "Detected meter is of type Logika4M" ) );
        //             std::vector< logika::protocols::M4::TagWriteData > tagsData;
        //             tagsData.emplace_back();
        //             tagsData.back().channel = 1;
        //             tagsData.back().ordinal = 2;
        //             tagsData.back().value   = std::make_shared< logika::Any >( logika::LocString{ LOCALIZED( "140" ) } );
        //             // tagsData.back().value   = std::make_shared< logika::Any >( uint32_t{ 1 } );
        //             tagsData.back().oper    = false;

        //             /// @todo Try dump traffic
        //             auto rcs = bus4->WriteParams4M( dm4m, nullptr, tagsData );
        //             for ( size_t i = 0; i < rcs.size(); ++i )
        //             {
        //                 LOG_WRITE( LOG_INFO, LOCALIZED( "Write Logika4M parameter ") << i
        //                     << LOCALIZED(" RC: " ) << rcs.at( i ) );
        //             }
        //         }
        //     }
        //     catch( const std::exception& e )
        //     {
        //         LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
        //     }
        // }

        // if ( detectedMeter )
        // {
        //     try
        //     {
        //         LOG_WRITE( LOG_DEBUG, LOCALIZED( "Try read Logika4M tags" ) );
        //         std::shared_ptr< logika::meters::Logika4M > dm4m
        //             = std::dynamic_pointer_cast< logika::meters::Logika4M >( detectedMeter );
        //         if ( dm4m )
        //         {
        //             const auto& tdfs = dm4m->GetTagsVault()->All();
        //             LOG_WRITE( LOG_DEBUG, LOCALIZED( "Reading Logika4M tags" ) );
        //             std::vector< int32_t > chans{};
        //             std::vector< int32_t > ords{};
        //             std::vector< bool > ops{};
        //             int i = 0;
        //             for ( auto tdf: tdfs )
        //             {
        //                 /// @todo Fix CRC for big response
        //                 if ( i++ < 30 )
        //                 {
        //                     continue;
        //                 }
        //                 chans.push_back( tdf->GetChannelDef().start );
        //                 ords.push_back( tdf->GetOrdinal() );
        //                 ops.push_back( false );
        //                 // LOG_WRITE( LOG_DEBUG, L"chn start:" << tdf->GetChannelDef().start << L" ord: " << tdf->GetOrdinal() );
        //             }

        //             auto tags = bus4->ReadTags4M( dm4m, nullptr, chans, ords, ops );
        //             LOG_WRITE( LOG_DEBUG, LOCALIZED( "Read Logika4M tags ended. Read " ) << tags.size() << LOCALIZED( " tags" ) );
        //             for ( int i = 0; i < tags.size(); ++i )
        //             {
        //                 LOG_WRITE( LOG_DEBUG, LOCALIZED( "" ) );
        //                 LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag name: " ) << tdfs[ i + 30 ]->GetName() );
        //                 LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag channel: " ) << tdfs[ i + 30 ]->GetChannelDef().prefix
        //                     << LOCALIZED( " " ) << tdfs[ i ]->GetChannelDef().start );
        //                 LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag ordinal: " ) << static_cast< uint32_t >( tdfs[ i ]->GetOrdinal() ) );
        //                 const auto& tag = tags[ i ];
        //                 LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag type: " ) << std::hex << tag.first );
        //                 if ( tag.first == 0x16 )
        //                 {
        //                     logika::LocString val{};
        //                     tag.second->TryCast< logika::LocString >( val );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
        //                 }
        //                 else if ( tag.first == 0x43 )
        //                 {
        //                     float val{};
        //                     tag.second->TryCast< float >( val );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
        //                 }
        //                 else if ( tag.first == 0x44 )
        //                 {
        //                     double val{};
        //                     tag.second->TryCast< double >( val );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
        //                 }
        //                 else if ( tag.first == 0x41 )
        //                 {
        //                     uint32_t val{};
        //                     tag.second->TryCast< uint32_t >( val );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
        //                 }
        //                 else if ( tag.first == 0x05 )
        //                 {
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: [empty]" ) );
        //                 }
        //                 else if ( tag.first == 0x41 )
        //                 {
        //                     uint32_t val{};
        //                     tag.second->TryCast< uint32_t >( val );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
        //                 }
        //                 else if ( tag.first == 0x47 || tag.first == 0x48 )
        //                 {
        //                     logika::LocString val{};
        //                     tag.second->TryCast< logika::LocString >( val );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
        //                 }
        //                 else if ( tag.first == 0x49 )
        //                 {
        //                     logika::TimeType val{};
        //                     tag.second->TryCast< logika::TimeType >( val );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: timestamp " ) << val );
        //                 }
        //                 else if ( tag.first == 0x04 )
        //                 {
        //                     logika::ByteVector val{};
        //                     tag.second->TryCast< logika::ByteVector >( val );
        //                     logika::LocString valStr = LOCALIZED( "{ " );
        //                     for ( logika::ByteType v: val )
        //                     {
        //                         valStr += logika::ToLocString( std::to_string( static_cast< uint32_t >( v ) & 0xFF ) ) + LOCALIZED( " " );
        //                     }
        //                     valStr += LOCALIZED( "}" );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << valStr );
        //                 }
        //                 else if ( tag.first == 0x4B )
        //                 {
        //                     std::vector< size_t > val{};
        //                     tag.second->TryCast< std::vector< size_t > >( val );
        //                     logika::LocString valStr = LOCALIZED( "{ " );
        //                     for ( size_t v: val )
        //                     {
        //                         valStr += logika::ToLocString( std::to_string( static_cast< uint32_t >( v ) & 0xFF ) ) + LOCALIZED( " " );
        //                     }
        //                     valStr += LOCALIZED( "}" );
        //                     LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << valStr );
        //                 }
        //                 LOG_WRITE( LOG_DEBUG, LOCALIZED( "" ) );
        //             }
        //         }
        //     }
        //     catch ( const std::exception& e )
        //     {
        //         LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
        //     }
        // }

        if ( detectedMeter )
        {
            try
            {
                LOG_WRITE( LOG_DEBUG, LOCALIZED( "Try read Logika4M tags" ) );
                std::shared_ptr< logika::meters::Logika4M > dm4m
                    = std::dynamic_pointer_cast< logika::meters::Logika4M >( detectedMeter );
                if ( dm4m )
                {
                    auto wktags = dm4m->GetWellKnownTags();
                    for ( auto& wktag: wktags )
                    {
                        LOG_WRITE( LOG_DEBUG, "Update well-known tags" );
                        bus4->UpdateTags( nullptr, nullptr, wktag.second );

                        for ( int i = 0; i < wktag.second.size(); ++i )
                        {
                            const auto& tag = wktag.second[ i ];
                            LOG_WRITE( LOG_DEBUG, LOCALIZED( "" ) );
                            LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag name: " ) << tag->GetName() );
                            LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag channel: " ) << tag->GetChannel().prefix
                                << LOCALIZED( " " ) << tag->GetChannel().no );
                            LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag ordinal: " ) << static_cast< uint32_t >( tag->GetOrdinal() ) );
                            {
                                logika::LocString val{};
                                if ( tag->TryGetValue< logika::LocString >( val ) )
                                {
                                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
                                    continue;
                                }
                            }

                            {
                                float val{};
                                if ( tag->TryGetValue< float >( val ) )
                                {
                                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
                                    continue;
                                }
                            }

                            {
                                double val{};
                                if ( tag->TryGetValue< double >( val ) )
                                {
                                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
                                    continue;
                                }
                            }

                            {
                                uint32_t val{};
                                if ( tag->TryGetValue< uint32_t >( val ) )
                                {
                                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << val );
                                    continue;
                                }
                            }

                            {
                                logika::TimeType val{};
                                if ( tag->TryGetValue< logika::TimeType >( val ) )
                                {
                                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: timestamp " ) << val );
                                    continue;
                                }
                            }

                            {
                                logika::ByteVector val{};
                                if ( tag->TryGetValue< logika::ByteVector >( val ) )
                                {
                                    logika::LocString valStr = LOCALIZED( "{ " );
                                    for ( logika::ByteType v: val )
                                    {
                                        valStr += logika::ToLocString( std::to_string( static_cast< uint32_t >( v ) & 0xFF ) ) + LOCALIZED( " " );
                                    }
                                    valStr += LOCALIZED( "}" );
                                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << valStr );
                                    continue;
                                }
                            }

                            {
                                std::vector< size_t > val{};
                                if ( tag->TryGetValue< std::vector< size_t > >( val ) )
                                {
                                    logika::LocString valStr = LOCALIZED( "{ " );
                                    for ( size_t v: val )
                                    {
                                        valStr += logika::ToLocString( std::to_string( static_cast< uint32_t >( v ) & 0xFF ) ) + LOCALIZED( " " );
                                    }
                                    valStr += LOCALIZED( "}" );
                                    LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << valStr );
                                    continue;
                                }
                            }
                            LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: <unknown type>" ) );
                        }
                    }
                }
            }
            catch ( const std::exception& e )
            {
                LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
            }
        }
    }
    // catch( const std::exception& e )
    // {
    //     LOG_WRITE( LOG_WARNING, LOCALIZED( "An error occured: " ) << logika::ToLocString( e.what() ) );
    // }

#if defined( _WIN32 ) || defined( _WIN64 )
    WSACleanup();
#endif
    return 0;
}


void LoadResources( const std::string& pathTo, logika::storage::StorageKeeper& sKeeper, bool silent )
{
    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveType >();
    auto atStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveType >();
    {
        logika::resources::Loader< logika::resources::ArchiveTypes > loader;
        auto resource = loader.Load( pathTo + "/ArchiveTypes.dat" );
        if ( resource )
        {
            auto types = logika::meters::converters::ArchiveTypeConverter::Convert( *resource );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << types.size() << LOCALIZED( " ArchiveType instances" ) );
            }
            for ( auto type: types )
            {
                if ( type )
                {
                    bool res = atStorage->AddItem( type->GetName(), type );
                    if ( !silent )
                    {
                        LOG_WRITE( LOG_INFO, LOCALIZED( "Add ArchiveType '" )
                            << type->GetName() << LOCALIZED( "' to storage: " )
                            << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                    }
                    std::shared_ptr< logika::meters::ArchiveType > item;
                    atStorage->GetItem( type->GetName(), item );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load ArchiveTypes" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::Meter >();
    auto mStorage = sKeeper.GetStorage< logika::LocString, logika::meters::Meter >();
    {
        logika::resources::Loader< logika::resources::DeviceList > loader;
        auto resource = loader.Load( pathTo + "/Devices.dat" );
        if ( resource )
        {
            auto devices = logika::meters::converters::DeviceConverter::Convert( *resource );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << devices.size() << LOCALIZED( " Device instances" ) );
            }
            for ( auto device: devices )
            {
                if ( device )
                {
                    bool res = mStorage->AddItem( device->GetCaption(), device );
                    if ( !silent )
                    {
                        LOG_WRITE( LOG_INFO, LOCALIZED( "Add Device '" )
                            << device->GetCaption() << LOCALIZED( "' to storage: " )
                            << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                    }
                    std::shared_ptr< logika::meters::Meter > item;
                    mStorage->GetItem( device->GetCaption(), item );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load DeviceList" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ChannelDef >();
    auto cdStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ChannelDef >();
    {
        logika::resources::Loader< logika::resources::ChannelList > loader;
        auto resource = loader.Load( pathTo + "/Channels.dat" );
        if ( resource )
        {
            auto channels = logika::meters::converters::ChannelConverter::Convert( *resource, mStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << channels.size() << LOCALIZED( " Channel instances" ) );
            }
            for ( auto channel: channels )
            {
                if ( channel )
                {
                    if ( !channel->meter )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " )
                            << channel->prefix << LOCALIZED( "(" ) << channel->description << LOCALIZED( ")" ) );
                    }
                    else
                    {
                        logika::LocString label = channel->meter->GetCaption() + LOCALIZED( "." ) + channel->prefix;
                        bool res = cdStorage->AddItem( label, channel );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add Channel '" )
                                << label << LOCALIZED( "' to storage: " )
                                << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ChannelDef > item;
                        cdStorage->GetItem( label, item );
                    }
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load Channels" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::TagDef4M >();
    auto m4tStorage = sKeeper.GetStorage< logika::LocString, logika::meters::TagDef4M >();
    {
        logika::resources::Loader< logika::resources::M4TagList > loader;
        auto resource = loader.Load( pathTo + "/M4Tags.dat" );
        if ( resource )
        {
            auto m4tags = logika::meters::converters::M4TagConverter::Convert( *resource, mStorage, cdStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << m4tags.size() << LOCALIZED( " M4Tags instances" ) );
            }
            for ( auto tag: m4tags )
            {
                if ( tag )
                {
                    if ( !tag->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << tag->GetName() );
                    }
                    else
                    {
                        logika::LocString label = tag->GetMeter()->GetCaption() + LOCALIZED( "." ) + tag->GetName();
                        bool res = m4tStorage->AddItem( label, tag );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add M4Tag '" )
                                << label << LOCALIZED( "' to storage: " )
                                << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::TagDef4M > item;
                        m4tStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert M4Tag" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load M4Tags" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::TagDef4L >();
    auto l4tStorage = sKeeper.GetStorage< logika::LocString, logika::meters::TagDef4L >();
    {
        logika::resources::Loader< logika::resources::L4TagList > loader;
        auto resource = loader.Load( pathTo + "/L4Tags.dat" );
        if ( resource )
        {
            auto l4tags = logika::meters::converters::L4TagConverter::Convert( *resource, mStorage, cdStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << l4tags.size() << LOCALIZED( " L4Tags instances" ) );
            }
            for ( auto tag: l4tags )
            {
                if ( tag )
                {
                    if ( !tag->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << tag->GetName() );
                    }
                    else
                    {
                        logika::LocString label = tag->GetMeter()->GetCaption() + LOCALIZED( "." ) + tag->GetName();
                        bool res = l4tStorage->AddItem( label, tag );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add L4Tag '" )
                                << label << LOCALIZED( "' to storage: " )
                                << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::TagDef4L > item;
                        l4tStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert L4Tag" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load L4Tags" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveDef4M >();
    auto m4aStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveDef4M >();
    {
        logika::resources::Loader< logika::resources::M4ArchiveList > loader;
        auto resource = loader.Load( pathTo + "/M4Archives.dat" );
        if ( resource )
        {
            auto m4archives = logika::meters::converters::M4ArchiveConverter::Convert( *resource, mStorage, cdStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << m4archives.size() << LOCALIZED( " M4Archive instances" ) );
            }
            for ( auto archive: m4archives )
            {
                if ( archive )
                {
                    if ( !archive->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << archive->GetName() );
                    }
                    else
                    {
                        logika::LocString label = archive->GetMeter()->GetCaption() + LOCALIZED( "." ) + archive->GetArchiveType()->GetName();
                        bool res = m4aStorage->AddItem( label, archive );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add M4Archive '" )
                                << label << LOCALIZED( "' to storage: " )
                                << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveDef4M > item;
                        m4aStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert M4Archive" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load M4Archives" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveDef4L >();
    auto l4aStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveDef4L >();
    {
        logika::resources::Loader< logika::resources::L4ArchiveList > loader;
        auto resource = loader.Load( pathTo + "/L4Archives.dat" );
        if ( resource )
        {
            auto l4archives = logika::meters::converters::L4ArchiveConverter::Convert( *resource, mStorage, cdStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << l4archives.size() << LOCALIZED( " L4Archive instances" ) );
            }
            for ( auto archive: l4archives )
            {
                if ( archive )
                {
                    if ( !archive->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << archive->GetName() );
                    }
                    else
                    {
                        logika::LocString label = archive->GetMeter()->GetCaption() + LOCALIZED( "." ) + archive->GetArchiveType()->GetName();
                        bool res = l4aStorage->AddItem( label, archive );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add L4Archive '" )
                                << label << LOCALIZED( "' to storage: " )
                                << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveDef4L > item;
                        l4aStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert L4Archive" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load L4Archives" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveFieldDef4M >();
    auto m4afStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveFieldDef4M >();
    {
        logika::resources::Loader< logika::resources::M4ArchiveFieldList > loader;
        auto resource = loader.Load( pathTo + "/M4ArchiveFields.dat" );
        if ( resource )
        {
            auto m4afs = logika::meters::converters::M4ArchiveFieldConverter::Convert(
                *resource, mStorage, m4aStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << m4afs.size() << LOCALIZED( " M4ArchiveField instances" ) );
            }
            for ( auto field: m4afs )
            {
                if ( field )
                {
                    if ( !field->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << field->GetName() );
                    }
                    else
                    {
                        logika::LocString label = field->GetMeter()->GetCaption()
                            + LOCALIZED( "." ) + field->GetArchiveType()->GetName()
                            + LOCALIZED( "." ) + field->GetName();
                        bool res = m4afStorage->AddItem( label, field );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add M4ArchiveField '" )
                                << label << LOCALIZED( "' to storage: " )
                                << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveFieldDef4M > item;
                        m4afStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert M4ArchiveField" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load M4ArchiveFields" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveFieldDef4L >();
    auto l4afStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveFieldDef4L >();
    {
        logika::resources::Loader< logika::resources::L4ArchiveFieldList > loader;
        auto resource = loader.Load( pathTo + "/L4ArchiveFields.dat" );
        if ( resource )
        {
            auto l4afs = logika::meters::converters::L4ArchiveFieldConverter::Convert(
                *resource, mStorage, l4aStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << l4afs.size() << LOCALIZED( " L4ArchiveField instances" ) );
            }
            for ( auto field: l4afs )
            {
                if ( field )
                {
                    if ( !field->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << field->GetName() );
                    }
                    else
                    {
                        logika::LocString label = field->GetMeter()->GetCaption()
                            + LOCALIZED( "." ) + field->GetArchiveType()->GetName()
                            + LOCALIZED( "." ) + field->GetName();
                        bool res = l4afStorage->AddItem( label, field );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add L4ArchiveField '" )
                                << label << LOCALIZED( "' to storage: " )
                                << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveFieldDef4L > item;
                        l4afStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert L4ArchiveField" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load L4ArchiveFields" ) );
        }
    }
} // LoadResources

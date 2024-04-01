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

#include <logika/common/misc.h>

/// @cond
#include <iostream>

#if defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#endif
/// @endcond

int main()
{
    logika::log::Logger& logger = logika::log::Logger::Instance();
    logger.SetLogType( logika::log::LogType::LogConsole );
    logger.SetLogLevel( logika::log::LogLevel::LogAll );

#if defined( _WIN32 ) || defined( _WIN64 )
    WSADATA wsaData;
    int wsaInitResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if ( 0 != wsaInitResult) {
        LOG_WRITE( LOG_ERROR, L"WSAStartup failed: " << wsaInitResult );
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
    con.Read( rdbuf, 5 );

    std::string comRd{ rdbuf.data(), rdbuf.size() };
    LOG_WRITE_MSG( LOG_INFO, L"" );
    LOG_WRITE( LOG_INFO, L"Read from " << logika::ToLocString( con.GetAddress() ) << L": " << comRd );
    LOG_WRITE_MSG( LOG_INFO, L"" );

    logika::connections::UdpConnection udpCon{ "127.0.0.1", 8083, 30000 };
    udpCon.Open();
    udpCon.Write( buffer );
    logika::ByteVector udpRdbuf;
    udpCon.Read( udpRdbuf, 6 );
    udpCon.Purge( logika::connections::PurgeFlags::TxRx );
    udpCon.Read( udpRdbuf, 5 );

    std::string udpRd{ udpRdbuf.data(), udpRdbuf.size() };
    LOG_WRITE( LOG_INFO, L"" );
    LOG_WRITE( LOG_INFO, L"Read from " << logika::ToLocString( udpCon.GetAddress() ) << L": " << udpRd );
    LOG_WRITE( LOG_INFO, L"" );

    logika::connections::TcpConnection tcpCon{ "127.0.0.1", 8084, 30000 };
    tcpCon.Open();
    tcpCon.Write( buffer );
    logika::ByteVector tcpRdbuf;
    tcpCon.Read( tcpRdbuf, 6 );
    tcpCon.Purge( logika::connections::PurgeFlags::TxRx );
    tcpCon.Read( tcpRdbuf, 5 );

    std::string tcpRd{ tcpRdbuf.data(), tcpRdbuf.size() };
    LOG_WRITE( LOG_INFO, L"" );
    LOG_WRITE( LOG_INFO, L"Read from " << logika::ToLocString( tcpCon.GetAddress() ) << L": " << tcpRd );
    LOG_WRITE( LOG_INFO, L"" );
#endif // if 0

    logika::meters::VitalInfo vi{ L"0x11", L"rev", L"serial", { L"eth0", L"eth1" }
        , static_cast< logika::ByteType >( 0xFF ), static_cast< logika::ByteType >( 0xFF )
        , static_cast< logika::ByteType >( 0xFF ), L"crc16" };
    logika::meters::HistoricalSeries hs{ 0x11, { { nullptr, 0, 0 } } };
    logika::meters::VQT vqt{};

#if 0
    logika::storage::StorageKeeper sKeeper = logika::storage::StorageKeeper::Instance();
    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveType >();
    auto atStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveType >();
    {
        logika::resources::Loader< logika::resources::ArchiveTypes > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/ArchiveTypes.dat" );
        if ( resource )
        {
            auto types = logika::meters::converters::ArchiveTypeConverter::Convert( *resource );
            LOG_WRITE( LOG_INFO, L"Converted " << types.size() << L" ArchiveType instances" );
            for ( auto type: types )
            {
                if ( type )
                {
                    LOG_WRITE( LOG_INFO, L"Add ArchiveType '" << type->GetName() << L"' to storage: "
                        << ( atStorage->AddItem( type->GetName(), type ) ? L"Success" : L"Failed" ) );
                    std::shared_ptr< logika::meters::ArchiveType > item;
                    atStorage->GetItem( type->GetName(), item );
                    std::wcout << item->GetDescription() << std::endl;
                    std::wcout << item->GetAcronym() << std::endl;
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load ArchiveTypes" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::Meter >();
    auto mStorage = sKeeper.GetStorage< logika::LocString, logika::meters::Meter >();
    {
        logika::resources::Loader< logika::resources::DeviceList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/Devices.dat" );
        if ( resource )
        {
            auto devices = logika::meters::converters::DeviceConverter::Convert( *resource );
            LOG_WRITE( LOG_INFO, L"Converted " << devices.size() << L" Device instances" );
            for ( auto device: devices )
            {
                if ( device )
                {
                    LOG_WRITE( LOG_INFO, L"Add Device '" << device->GetCaption() << L"' to storage: "
                        << ( mStorage->AddItem( device->GetCaption(), device ) ? L"Success" : L"Failed" ) );
                    std::shared_ptr< logika::meters::Meter > item;
                    mStorage->GetItem( device->GetCaption(), item );
                    std::wcout << item->GetDescription() << std::endl;
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load DeviceList" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ChannelDef >();
    auto cdStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ChannelDef >();
    {
        logika::resources::Loader< logika::resources::ChannelList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/Channels.dat" );
        if ( resource )
        {
            auto channels = logika::meters::converters::ChannelConverter::Convert( *resource, mStorage );
            LOG_WRITE( LOG_INFO, L"Converted " << channels.size() << L" Channel instances" );
            for ( auto channel: channels )
            {
                if ( channel )
                {
                    if ( !channel->meter )
                    {
                        LOG_WRITE( LOG_ERROR, L"No meter instance found for "
                            << channel->prefix << L"(" << channel->description << L")" );
                    }
                    else
                    {
                        logika::LocString label = channel->meter->GetCaption() + L"_" + channel->prefix;
                        LOG_WRITE( LOG_INFO, L"Add Channel '" << label << L"' to storage: "
                            << ( cdStorage->AddItem( label, channel ) ? L"Success" : L"Failed" ) );
                        std::shared_ptr< logika::meters::ChannelDef > item;
                        cdStorage->GetItem( label, item );
                    }
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load Channels" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::TagDef4M >();
    auto m4tStorage = sKeeper.GetStorage< logika::LocString, logika::meters::TagDef4M >();
    {
        logika::resources::Loader< logika::resources::M4TagList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/M4Tags.dat" );
        if ( resource )
        {
            auto m4tags = logika::meters::converters::M4TagConverter::Convert( *resource, mStorage, cdStorage );
            LOG_WRITE( LOG_INFO, L"Converted " << m4tags.size() << L" M4Tags instances" );
            for ( auto tag: m4tags )
            {
                if ( tag )
                {
                    if ( !tag->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, L"No meter instance found for " << tag->GetName() );
                    }
                    else
                    {
                        logika::LocString label = tag->GetMeter()->GetCaption() + L"_" + tag->GetName();
                        LOG_WRITE( LOG_INFO, L"Add M4Tag '" << label << L"' to storage: "
                            << ( m4tStorage->AddItem( label, tag ) ? L"Success" : L"Failed" ) );
                        std::shared_ptr< logika::meters::TagDef4M > item;
                        m4tStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, L"Unable to convert M4Tag" );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load M4Tags" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::TagDef4L >();
    auto l4tStorage = sKeeper.GetStorage< logika::LocString, logika::meters::TagDef4L >();
    {
        logika::resources::Loader< logika::resources::L4TagList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/L4Tags.dat" );
        if ( resource )
        {
            auto l4tags = logika::meters::converters::L4TagConverter::Convert( *resource, mStorage, cdStorage );
            LOG_WRITE( LOG_INFO, L"Converted " << l4tags.size() << L" L4Tags instances" );
            for ( auto tag: l4tags )
            {
                if ( tag )
                {
                    if ( !tag->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, L"No meter instance found for " << tag->GetName() );
                    }
                    else
                    {
                        logika::LocString label = tag->GetMeter()->GetCaption() + L"_" + tag->GetName();
                        LOG_WRITE( LOG_INFO, L"Add L4Tag '" << label << L"' to storage: "
                            << ( l4tStorage->AddItem( label, tag ) ? L"Success" : L"Failed" ) );
                        std::shared_ptr< logika::meters::TagDef4L > item;
                        l4tStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, L"Unable to convert L4Tag" );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load L4Tags" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveDef4M >();
    auto m4aStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveDef4M >();
    {
        logika::resources::Loader< logika::resources::M4ArchiveList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/M4Archives.dat" );
        if ( resource )
        {
            auto m4archives = logika::meters::converters::M4ArchiveConverter::Convert( *resource, mStorage, cdStorage, atStorage );
            LOG_WRITE( LOG_INFO, L"Converted " << m4archives.size() << L" M4Archive instances" );
            for ( auto archive: m4archives )
            {
                if ( archive )
                {
                    if ( !archive->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, L"No meter instance found for " << archive->GetName() );
                    }
                    else
                    {
                        logika::LocString label = archive->GetMeter()->GetCaption() + L"_" + archive->GetArchiveType()->GetName();
                        LOG_WRITE( LOG_INFO, L"Add M4Archive '" << label << L"' to storage: "
                            << ( m4aStorage->AddItem( label, archive ) ? L"Success" : L"Failed" ) );
                        std::shared_ptr< logika::meters::ArchiveDef4M > item;
                        m4aStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, L"Unable to convert M4Archive" );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load M4Archives" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveDef4L >();
    auto l4aStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveDef4L >();
    {
        logika::resources::Loader< logika::resources::L4ArchiveList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/L4Archives.dat" );
        if ( resource )
        {
            auto l4archives = logika::meters::converters::L4ArchiveConverter::Convert( *resource, mStorage, cdStorage, atStorage );
            LOG_WRITE( LOG_INFO, L"Converted " << l4archives.size() << L" L4Archive instances" );
            for ( auto archive: l4archives )
            {
                if ( archive )
                {
                    if ( !archive->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, L"No meter instance found for " << archive->GetName() );
                    }
                    else
                    {
                        /// @todo GetName() -> GetArchiveType()->GetName()
                        logika::LocString label = archive->GetMeter()->GetCaption() + L"_" + archive->GetArchiveType()->GetName();
                        LOG_WRITE( LOG_INFO, L"Add L4Archive '" << label << L"' to storage: "
                            << ( l4aStorage->AddItem( label, archive ) ? L"Success" : L"Failed" ) );
                        std::shared_ptr< logika::meters::ArchiveDef4L > item;
                        l4aStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, L"Unable to convert L4Archive" );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load L4Archives" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveFieldDef4M >();
    auto m4afStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveFieldDef4M >();
    {
        logika::resources::Loader< logika::resources::M4ArchiveFieldList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/M4ArchiveFields.dat" );
        if ( resource )
        {
            auto m4afs = logika::meters::converters::M4ArchiveFieldConverter::Convert(
                *resource, mStorage, m4aStorage, atStorage );
            LOG_WRITE( LOG_INFO, L"Converted " << m4afs.size() << L" M4ArchiveField instances" );
            for ( auto field: m4afs )
            {
                if ( field )
                {
                    if ( !field->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, L"No meter instance found for " << field->GetName() );
                    }
                    else
                    {
                        logika::LocString label = field->GetMeter()->GetCaption()
                            + L"_" + field->GetArchiveType()->GetName()
                            + L"_" + field->GetName();
                        LOG_WRITE( LOG_INFO, L"Add M4ArchiveField '" << label << L"' to storage: "
                            << ( m4afStorage->AddItem( label, field ) ? L"Success" : L"Failed" ) );
                        std::shared_ptr< logika::meters::ArchiveFieldDef4M > item;
                        m4afStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, L"Unable to convert M4ArchiveField" );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load M4ArchiveFields" );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveFieldDef4L >();
    auto l4afStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveFieldDef4L >();
    {
        logika::resources::Loader< logika::resources::L4ArchiveFieldList > loader;
        auto resource = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/L4ArchiveFields.dat" );
        if ( resource )
        {
            auto l4afs = logika::meters::converters::L4ArchiveFieldConverter::Convert(
                *resource, mStorage, l4aStorage, atStorage );
            LOG_WRITE( LOG_INFO, L"Converted " << l4afs.size() << L" L4ArchiveField instances" );
            for ( auto field: l4afs )
            {
                if ( field )
                {
                    if ( !field->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, L"No meter instance found for " << field->GetName() );
                    }
                    else
                    {
                        logika::LocString label = field->GetMeter()->GetCaption()
                            + L"_" + field->GetArchiveType()->GetName()
                            + L"_" + field->GetName();
                        LOG_WRITE( LOG_INFO, L"Add L4ArchiveField '" << label << L"' to storage: "
                            << ( l4afStorage->AddItem( label, field ) ? L"Success" : L"Failed" ) );
                        std::shared_ptr< logika::meters::ArchiveFieldDef4L > item;
                        l4afStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, L"Unable to convert L4ArchiveField" );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, L"Unable to load L4ArchiveFields" );
        }
    }

    auto cdKeys = cdStorage->GetKeys();
    for ( auto key: cdKeys )
    {
        LOG_WRITE( LOG_INFO, L"ChannelDef key '" << key << L"'" );
    }


    logika::meters::ArchiveFieldDefSettings afdSettings1;
    afdSettings1.ordinal = 10;
    afdSettings1.name = L"tag1";
    atStorage->GetItem( L"Hour", afdSettings1.archiveType );

    logika::meters::ChannelDef cdef1{ nullptr, L"chn", 0, 10, L"some channel" };
    std::shared_ptr< logika::meters::ArchiveFieldDef > afd1 =
        std::make_shared< logika::meters::ArchiveFieldDef >( cdef1, afdSettings1 );

    logika::meters::ArchiveFieldDefSettings afdSettings2;
    afdSettings2.ordinal = 1;
    afdSettings2.name = L"tag2";
    atStorage->GetItem( L"Hour", afdSettings2.archiveType );

    logika::meters::ChannelDef cdef2{ nullptr, L"chn1", 0, 10, L"some channel 2" };
    std::shared_ptr< logika::meters::ArchiveFieldDef > afd2 =
        std::make_shared< logika::meters::ArchiveFieldDef >( cdef2, afdSettings2 );

    logika::meters::ArchiveFieldDefSettings afdSettings3;
    afdSettings3.ordinal = 2;
    afdSettings3.name = L"TM";
    atStorage->GetItem( L"Hour", afdSettings3.archiveType );

    logika::meters::ChannelDef cdef3{ nullptr, L"tmChn", 0, 10, L"timestamp channel" };
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
    atStorage->GetItem( logika::LocString{ L"Month" }, monthArchive );
    mStorage->GetItem( logika::LocString{ L"LGK410" }, lgk410meter );
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
            LOG_WRITE( LOG_INFO, L"DataTable column '" << field->GetName() << L"'" );
        }
    }

    auto meter = logika::meters::Meter::Create< logika::meters::Meter >();
#endif // if 0

#if defined( _WIN32 ) || defined( _WIN64 )
    WSACleanup();
#endif
    return 0;
}

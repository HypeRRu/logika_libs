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

#include <logika/meters/data_table.hpp>
#include <logika/meters/interval_archive.h>

#include <logika/meters/meter.h>

#include <logika/common/misc.h>

#include <iostream>

#if defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#endif

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

    logika::meters::VitalInfo vi{ "0x11", "rev", "serial", { "eth0", "eth1" }
        , static_cast< logika::ByteType >( 0xFF ), static_cast< logika::ByteType >( 0xFF )
        , static_cast< logika::ByteType >( 0xFF ), "crc16" };
    logika::meters::HistoricalSeries hs{ 0x11, { { nullptr, 0, 0 } } };
    logika::meters::VQT vqt{};

    logika::storage::StorageKeeper sKeeper = logika::storage::StorageKeeper::Instance();
    sKeeper.CreateStorage< std::string, logika::meters::ArchiveType >();
    auto atStorage = sKeeper.GetStorage< std::string, logika::meters::ArchiveType >();
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
                    LOG_WRITE( LOG_INFO, L"Add ArchiveType '" << logika::ToLocString( type->GetName() ) << L"' to storage: "
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

    sKeeper.CreateStorage< std::string, logika::meters::Meter >();
    auto mStorage = sKeeper.GetStorage< std::string, logika::meters::Meter >();
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
                    LOG_WRITE( LOG_INFO, L"Add Device '" << logika::ToLocString( device->GetCaption() ) << L"' to storage: "
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


    logika::meters::ArchiveFieldDefSettings afdSettings1;
    afdSettings1.ordinal = 10;
    afdSettings1.name = "tag1";
    atStorage->GetItem( "Hour", afdSettings1.archiveType );

    logika::meters::ChannelDef cdef1{ nullptr, "chn", 0, 10, "some channel" };
    logika::meters::ArchiveFieldDef afd1{ cdef1, afdSettings1 };

    logika::meters::ArchiveFieldDefSettings afdSettings2;
    afdSettings2.ordinal = 1;
    afdSettings2.name = "tag2";
    atStorage->GetItem( "Hour", afdSettings2.archiveType );

    logika::meters::ChannelDef cdef2{ nullptr, "chn1", 0, 10, "some channel 2" };
    logika::meters::ArchiveFieldDef afd2{ cdef2, afdSettings2 };

    logika::meters::ArchiveFieldDefSettings afdSettings3;
    afdSettings3.ordinal = 2;
    afdSettings3.name = "TM";
    atStorage->GetItem( "Hour", afdSettings3.archiveType );

    logika::meters::ChannelDef cdef3{ nullptr, "tmChn", 0, 10, "timestamp channel" };
    logika::meters::ArchiveFieldDef afd3{ cdef3, afdSettings3 };

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
    atStorage->GetItem( std::string{ "Month" }, monthArchive );
    mStorage->GetItem( std::string{ "LGK410" }, lgk410meter );
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
            LOG_WRITE( LOG_INFO, L"DataTable column '" << logika::ToLocString( field->GetName() ) << L"'" );
        }
    }

    auto meter = logika::meters::Meter::Create< logika::meters::Meter >();

#if defined( _WIN32 ) || defined( _WIN64 )
    WSACleanup();
#endif
    return 0;
}

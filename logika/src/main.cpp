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
        LOG_WRITE( LOG_ERROR,  "WSAStartup failed: " << wsaInitResult );
        return 1;
    }
#endif

//     logika::ByteVector buffer{ 'b', 'u', 'f', 'f', 'e', 'r', '\n', '\0' };
// #if defined( __linux__ ) || defined( __APPLE__ )
//     logika::connections::SerialPortConnection con{ "/tmp/ttyV0", 1000 };
// #endif
// #if defined( _WIN32 ) || defined( _WIN64 )
//     logika::connections::SerialPortConnection con{ "\\\\.\\COM10", 1000 };
// #endif
//     con.SetBaudRate( logika::connections::BaudRate::Rate110 );
//     con.SetStopBits( logika::connections::StopBits::StopBitsTwo );
//     con.SetDataBits( logika::connections::DataBits::DataBits5 );
//     con.SetParity( logika::connections::Parity::ParityEven );
//     con.Open();
//     con.Write( buffer );
//     con.Purge( logika::connections::PurgeFlags::TxRx );
//     logika::ByteVector rdbuf;
//     con.Read( rdbuf, 5 );

//     std::string comRd{ rdbuf.data(), rdbuf.size() };
//     LOG_WRITE( LOG_INFO, "" );
//     LOG_WRITE( LOG_INFO, "Read from " << con.GetAddress() << ": " << comRd );
//     LOG_WRITE( LOG_INFO, "" );

//     logika::connections::UdpConnection udpCon{ "127.0.0.1", 8083, 30000 };
//     udpCon.Open();
//     udpCon.Write( buffer );
//     logika::ByteVector udpRdbuf;
//     udpCon.Read( udpRdbuf, 6 );
//     udpCon.Purge( logika::connections::PurgeFlags::TxRx );
//     udpCon.Read( udpRdbuf, 5 );

//     std::string udpRd{ udpRdbuf.data(), udpRdbuf.size() };
//     LOG_WRITE( LOG_INFO, "" );
//     LOG_WRITE( LOG_INFO, "Read from " << udpCon.GetAddress() << ": " << udpRd );
//     LOG_WRITE( LOG_INFO, "" );

//     logika::connections::TcpConnection tcpCon{ "127.0.0.1", 8084, 30000 };
//     tcpCon.Open();
//     tcpCon.Write( buffer );
//     logika::ByteVector tcpRdbuf;
//     tcpCon.Read( tcpRdbuf, 6 );
//     tcpCon.Purge( logika::connections::PurgeFlags::TxRx );
//     tcpCon.Read( tcpRdbuf, 5 );

//     std::string tcpRd{ tcpRdbuf.data(), tcpRdbuf.size() };
//     LOG_WRITE( LOG_INFO, "" );
//     LOG_WRITE( LOG_INFO, "Read from " << tcpCon.GetAddress() << ": " << tcpRd );
//     LOG_WRITE( LOG_INFO, "" );

    logika::meters::VitalInfo vi{ "0x11", "rev", "serial", { "eth0", "eth1" }
        , static_cast< logika::ByteType >( 0xFF ), static_cast< logika::ByteType >( 0xFF )
        , static_cast< logika::ByteType >( 0xFF ), "crc16" };
    logika::meters::HistoricalSeries hs{ 0x11, { { nullptr, 0, 0 } } };
    logika::meters::VQT vqt{};

    const logika::meters::ArchiveType& hr = logika::meters::ArchiveType::Hour;

    logika::meters::ArchiveFieldDefSettings afdSettings;
    afdSettings.ordinal = 10;
    afdSettings.archiveTypeName = "Hour";

    logika::meters::ChannelDef cdef{ nullptr, "chn", 0, 10, "some channel" };
    logika::meters::ArchiveFieldDef afd{ cdef, afdSettings };

    logika::resources::Loader< logika::resources::L4ArchiveFieldList > loader;
    // logika::resources::Loader< std::string > loader;
    auto l4ArchiveFields = loader.Load( "/home/hyper/prog/diploma/logika_libs/migration/binary/L4ArchiveFields.dat" );
    LOG_WRITE( LOG_INFO, l4ArchiveFields->list_size() );

#if defined( _WIN32 ) || defined( _WIN64 )
    WSACleanup();
#endif
    return 0;
}

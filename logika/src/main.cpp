#include <logika/connections/serial/serial_port_connection.h>
#include <logika/connections/network/udp_connection.h>
#include <logika/connections/network/tcp_connection.h>
#include <logika/log/logger.h>
#include <logika/log/defines.h>

#include <iostream>

#if defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#endif

int main()
{
#if defined( _WIN32 ) || defined( _WIN64 )
    WSADATA wsaData;
    int wsaInitResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if ( 0 != wsaInitResult) {
        printf( "WSAStartup failed: %d\n", wsaInitResult );
        return 1;
    }
#endif

    logika::log::Logger& logger = logika::log::Logger::Instance();
    logger.SetLogType( logika::log::LogType::LogConsole );
    logger.SetLogLevel( logika::log::LogLevel::LogAll );

    logika::ByteVector buffer{ 'b', 'u', 'f', 'f', 'e', 'r', '\n', '\0' };
#if defined( __linux__ ) || defined( __APPLE__ )
    logika::connections::SerialPortConnection con{ "/dev/pts/3", 1000 };
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

    logika::connections::UdpConnection udpCon{ "127.0.0.1", 8083, 1000 };
    udpCon.Open();
    udpCon.Write( buffer );
    logika::ByteVector udpRdbuf;
    udpCon.Read( udpRdbuf, 6 );
    udpCon.Purge( logika::connections::PurgeFlags::TxRx );
    udpCon.Read( udpRdbuf, 5 );

    logika::connections::TcpConnection tcp{ "127.0.0.1", 8084, 1000 };
    tcp.Open();
    tcp.Write( buffer );
    logika::ByteVector tcpRdbuf;
    tcp.Read( tcpRdbuf, 6 );
    tcp.Purge( logika::connections::PurgeFlags::TxRx );
    tcp.Read( tcpRdbuf, 5 );

#if defined( _WIN32 ) || defined( _WIN64 )
    WSACleanup();
#endif
    return 0;
}

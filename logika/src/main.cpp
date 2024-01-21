#include <logika/connections/serial/serial_port_connection.h>
#include <logika/log/logger.h>
#include <logika/log/defines.h>

#include <iostream>

int main()
{
    logika::log::Logger& logger = logika::log::Logger::Instance();
    logger.SetLogType( logika::log::LogType::LogConsole );
    logger.SetLogLevel( logika::log::LogLevel::LogAll );

    logika::connections::SerialPortConnection con{ "/dev/pts/2", 1000 };
    con.SetBaudRate( logika::connections::BaudRate::Rate110 );
    con.SetStopBits( logika::connections::StopBits::StopBitsTwo );
    con.SetDataBits( logika::connections::DataBits::DataBits5 );
    con.SetParity( logika::connections::Parity::ParityEven );
    con.Open();
    con.Write( { 'b', 'u', 'f', 'f', 'e', 'r', '\n' } );
    con.Purge( logika::connections::PurgeFlags::TxRx );

    logika::ByteVector rdbuf;
    con.Read( rdbuf, 5 );
    return 0;
}

#include <logika/connections/serial/serial_connection.h>
#include <logika/log/logger.h>

#include <iostream>

int main()
{
    logika::log::Logger& logger = logika::log::Logger::Instance();
    logger.SetLogType( logika::log::LogType::LogConsole );
    logger.SetLogLevel( logika::log::LogLevel::LogAll );

    logika::connections::SerialConnection con{ "/dev/ttys0", 0 };
    con.SetBaudRate( logika::connections::BaudRate::Rate115200 );
    con.SetStopBits( logika::connections::StopBits::StopBitsTwo );
    con.SetDataBits( logika::connections::DataBits::DataBits5 );
    con.SetParity( logika::connections::Parity::ParityEven );
    con.Open();
    con.Write( { 'b', 'u', 'f', 'f', 'e', 'r', '\n' } );
    con.Purge( logika::connections::PurgeFlags::TxRx );

    logika::ByteVector rdbuf;
    con.Read( rdbuf, 12 );
    return 0;
}

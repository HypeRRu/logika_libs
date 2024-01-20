#include <logika/connections/connection.h>
#include <logika/log/logger.h>

#include <iostream>

int main()
{
    logika::log::Logger& logger = logika::log::Logger::Instance();
    logger.SetLogType( logika::log::LogType::LogConsole );
    logger.SetLogLevel( logika::log::LogLevel::LogError );

    logika::connections::Connection con{ "localhost", 0 };
    con.Open();
    con.Write( { 'b', 'u', 'f', 'f', 'e', 'r', '\n' } );
    return 0;
}

/// @file Реализация класса логгера
/// @copyright HypeRRu 2024

#include <logika/log/logger.h>

#include <array>
#include <chrono>
#include <iostream>

#include <ctime>

namespace // anonymous
{

const std::array< std::string, logika::log::LogLevel::Count > logLevelStrings{
      "DISABLED"
    , "ERROR"
    , "WARNING"
    , "INFO"
    , "DEBUG"
};

std::string GetCurrentTime()
{
    auto currentTime = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
    std::string timeNow = std::ctime( &currentTime );
    timeNow.pop_back(); ///< Удаление переноса строки
    return timeNow;
} // GetCurrentTime

} // anonymous namespace


namespace logika
{

namespace log
{

Logger::Logger()
    : logType_{ LogType::LogDisable }
    , maxLevel_{ LogLevel::LogDisable }
    , fileStream_{}
    , mtx_{}
{} // Logger


Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
} // Instance


void Logger::SetLogType( LogType::Type type )
{
    {
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
        logType_ = type;
    }

    CheckLogLevel();
} // SetLogType


void Logger::SetLogFile( const std::string& filePath, bool append )
{
    if ( filePath.empty() )
    {
        return;
    }

    {
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
        if ( fileStream_.is_open() )
        {
            fileStream_.close();
        }

        fileStream_.open( filePath, append ? std::ios::app : std::ios::trunc );
    }

    CheckLogLevel();
} // SetLogFile


void Logger::SetLogLevel( LogLevel::Type level )
{
    {
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
        maxLevel_   = level;
    }

    CheckLogLevel();
} // SetLogLevel


void Logger::Write( LogLevel::Type level, const std::string& message, const std::string& funcName )
{
    const std::string msgToWrite = GetCurrentTime() + " [" + logLevelStrings[ level ] + "] "
        + ( !funcName.empty() ? ( funcName + "\t| " ) : "" ) + message + "\n";

    std::unique_lock< std::mutex > lockGuard{ mtx_ };
    if ( level == LogLevel::LogDisable || level > maxLevel_ )
    {
        return;
    }

    if ( logType_ & LogType::LogConsole )
    {
        std::cout << msgToWrite;
    }
    if ( ( logType_ & LogType::LogFile ) && fileStream_.is_open() )
    {
        fileStream_ << msgToWrite;
    }
} // Write


void Logger::CheckLogLevel()
{
    Write( LogLevel::LogError, "Logging error messages" );
    Write( LogLevel::LogWarning, "Logging warning messages" );
    Write( LogLevel::LogInfo, "Logging info messages" );
    Write( LogLevel::LogDebug, "Logging debug messages" );
} // CheckLogLevel

} // namespace log

} // namespace logika

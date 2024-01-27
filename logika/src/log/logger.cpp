/// @file Реализация класса логгера
/// @copyright HypeRRu 2024

#include <logika/log/logger.h>

#include <logika/common/misc.h>

#include <array>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <ctime>

namespace // anonymous
{

const std::array< std::string, logika::log::LogLevel::Count > logLevelStrings{
      "DISABLED"
    , " ERROR "
    , "  WARN "
    , "  INFO "
    , " DEBUG "
};

std::string GetCurrentTime()
{
    auto currentTime = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
#if defined( __linux__ ) || defined( __APPLE__ )
    std::string timeNow = std::ctime( &currentTime );
    timeNow.pop_back(); ///< Удаление переноса строки
    return timeNow;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
    constexpr size_t timeStrSize = 26;
    char timeStr[ timeStrSize ];
    errno_t error;
    if ( 0 != ( error = ctime_s( timeStr, timeStrSize, &currentTime ) ) )
    {
        std::cerr << logika::SafeStrError( error ) << '\n';
        return "";
    }
    return timeStr;
#endif // defined( _WIN32 ) || defined( _WIN64 )
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
    std::stringstream msgToWrite;
    msgToWrite << GetCurrentTime() << " ";
    msgToWrite << "[" << logLevelStrings[ level ] << "] ";
    if ( !funcName.empty() )
    {
        msgToWrite << funcName + " | ";
    }
    msgToWrite << message << '\n';
    // const std::string msgToWrite = GetCurrentTime() + " [" + logLevelStrings[ level ] + "] "
    //     + ( !funcName.empty() ? ( funcName + "\t| " ) : "" ) + message + "\n";

    std::unique_lock< std::mutex > lockGuard{ mtx_ };
    if ( level == LogLevel::LogDisable || level > maxLevel_ )
    {
        return;
    }

    if ( logType_ & LogType::LogConsole )
    {
        std::cout << msgToWrite.str();
    }
    if ( ( logType_ & LogType::LogFile ) && fileStream_.is_open() )
    {
        fileStream_ << msgToWrite.str();
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

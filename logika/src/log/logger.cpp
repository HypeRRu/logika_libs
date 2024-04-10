/// @file Реализация класса логгера
/// @copyright HypeRRu 2024

#include <logika/log/logger.h>

#include <logika/common/misc.h>

/// @cond
#include <array>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>

#include <ctime>
/// @endcond

namespace // anonymous
{

const std::array< logika::LocString, logika::log::LogLevel::Count > logLevelStrings{
      LOCALIZED( "DISABLED" )
    , LOCALIZED( " ERROR "  )
    , LOCALIZED( "  WARN "  )
    , LOCALIZED( "  INFO "  )
    , LOCALIZED( " DEBUG "  )
};

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
{
    InitLocale();
} // Logger


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
        fileStream_.imbue( std::locale() );
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


void Logger::Write( LogLevel::Type level, const LocString& message, const std::string& funcName )
{
    LocStringStream msgToWrite;
    msgToWrite << GetTimeString() << LOCALIZED( ' ' );
    msgToWrite << LOCALIZED( '[' ) << logLevelStrings[ level ] << LOCALIZED( "] " );
    if ( !funcName.empty() )
    {
        msgToWrite << ToLocString( funcName ) << LOCALIZED( " | " );
    }
    msgToWrite << message << '\n';
    
    std::unique_lock< std::mutex > lockGuard{ mtx_ };
    if ( level == LogLevel::LogDisable || level > maxLevel_ )
    {
        return;
    }

    if ( logType_ & LogType::LogConsole )
    {
        std::wcout << msgToWrite.str();
    }
    if ( ( logType_ & LogType::LogFile ) && fileStream_.is_open() )
    {
        fileStream_ << msgToWrite.str();
    }
} // Write( LogLevel::Type, LocString, LocString )


void Logger::CheckLogLevel()
{
    Write( LogLevel::LogError, LOCALIZED( "Logging error messages" ) );
    Write( LogLevel::LogWarning, LOCALIZED( "Logging warning messages" ) );
    Write( LogLevel::LogInfo, LOCALIZED( "Logging info messages" ) );
    Write( LogLevel::LogDebug, LOCALIZED( "Logging debug messages" ) );
} // CheckLogLevel


void Logger::InitLocale()
{
    std::locale::global( std::locale( "" ) );
    std::wcout.imbue( std::locale() );
} // InitLocale

} // namespace log

} // namespace logika

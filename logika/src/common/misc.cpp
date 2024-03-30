/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#include <logika/common/misc.h>

#include <sstream>
#include <cstring>
#include <chrono>
#include <algorithm>

#include <ctime>
#include <cctype>


namespace // anonymous
{

bool CharCaseEq( char lhs, char rhs )
{
    return std::tolower( static_cast< unsigned char >( lhs ) )
        == std::tolower( static_cast< unsigned char >( rhs ) );
} // CharCaseEq

} // anonymous namespace


namespace logika
{

LocString ToLocString( const std::string& str )
{
    static LocConverter locStrConverter{};
    return locStrConverter.from_bytes( str );
} // ToLocString( const std::string& str )


const LocString& ToLocString( const LocString& str )
{
    return str;
} // ToLocString( const LocString& str )


std::string SafeStrError( int error )
{
    constexpr size_t errorStrSize = 1024;
    std::stringstream ss;
    char buffer[ errorStrSize ] = { 0 };
#if ( _POSIX_C_SOURCE >= 200112L ) && ! _GNU_SOURCE
    if ( !strerror_r( error, buffer, errorStrSize ) )
    {
        return "";
    }
    ss << buffer;
#elif _GNU_SOURCE
    ss << strerror_r( error, buffer, errorStrSize );
#elif defined( _WIN32 ) || defined( _WIN64 )
    strerror_s( buffer, errorStrSize, error );
    ss << buffer;
#endif
    ss << " (" << error << ")";
    return ss.str();
} // SafeStrError


std::string GetTimeString( TimeType timestamp )
{
    time_t stamp = static_cast< time_t >( timestamp );
    if ( 0 == stamp )
    {
        stamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
    }
    
    struct tm timeStruct;
#if defined( _POSIX_C_SOURCE ) || defined( _BSD_SOURCE ) || defined( _SVID_SOURCE )
    (void) localtime_r( &stamp, &timeStruct );
#elif defined( _WIN32 ) || defined( _WIN64 )
    (void) localtime_s( &timeStruct, &stamp );
#else
    /// MT-unsafe
    timeStruct = *std::localtime( &stamp );
#endif

    constexpr size_t bufSize = sizeof( "dd.mm.yyyy HH:MM:SS TMZ" );
    char buffer[ bufSize ] = { 0 };
    std::strftime( buffer, bufSize, "%d.%m.%Y %H:%M:%S %Z", &timeStruct );

    return std::string{ buffer };
} // GetTimeString


bool StrCaseEq( const std::string& lhs, const std::string& rhs )
{
    return lhs.size() == rhs.size()
        && std::equal( lhs.cbegin(), lhs.cend(), rhs.cbegin(), CharCaseEq );
} // StrCaseEq


std::string Trim( const std::string& str, const std::string& trimChars )
{
    size_t leftPos = str.find_first_not_of( trimChars );
    if ( std::string::npos == leftPos )
    {
        return "";
    }
    size_t rightPos = str.find_last_not_of( trimChars );
    return str.substr( leftPos, rightPos - leftPos );
} // Trim

} // namespace logika

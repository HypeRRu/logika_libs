/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#include <logika/common/misc.h>

#include <sstream>
#include <cstring>
#include <chrono>

#include <ctime>

namespace logika
{

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

} // namespace logika

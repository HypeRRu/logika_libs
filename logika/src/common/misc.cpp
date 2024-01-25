/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#include <logika/common/misc.h>

#include <sstream>
#include <cstring>

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

} // namespace logika

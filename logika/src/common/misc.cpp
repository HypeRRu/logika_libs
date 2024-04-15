/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#include <logika/common/misc.h>

/// @cond
#include <sstream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <iomanip>

#include <ctime>
#include <cctype>
#include <cwctype>
/// @endcond


namespace // anonymous
{

bool LocCharCaseEq( logika::LocChar lhs, logika::LocChar rhs )
{
    return std::towlower( lhs )
        == std::towlower( rhs );
} // CharCaseEq

constexpr size_t byteSize = 8;

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


LocString SafeStrError( int error )
{
    constexpr size_t errorStrSize = 1024;
    LocStringStream ss;
    char buffer[ errorStrSize ] = { 0 };
#if ( _POSIX_C_SOURCE >= 200112L ) && ! _GNU_SOURCE
    if ( !strerror_r( error, buffer, errorStrSize ) )
    {
        return LOCALIZED( "" );
    }
    ss << ToLocString( buffer );
#elif _GNU_SOURCE
    ss << ToLocString( strerror_r( error, buffer, errorStrSize ) );
#elif defined( _WIN32 ) || defined( _WIN64 )
    strerror_s( buffer, errorStrSize, error );
    ss << ToLocString( buffer );
#endif
    ss << LOCALIZED( " (" ) << error << LOCALIZED( ")" );
    return ss.str();
} // SafeStrError


TimeType GetCurrentTimestamp()
{
    return std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() ) * 1000;
} // GetCurrentTimestamp


struct tm GetTimeStruct( TimeType timestamp, uint16_t* millis )
{
    time_t stamp = static_cast< time_t >( timestamp );
    if ( 0 == stamp )
    {
        stamp = GetCurrentTimestamp();
    }
    if ( millis )
    {
        *millis = stamp % 1000;
    }
    stamp /= 1000; /// мс -> с
    
    struct tm timeStruct;
#if defined( _POSIX_C_SOURCE ) || defined( _BSD_SOURCE ) || defined( _SVID_SOURCE )
    (void) localtime_r( &stamp, &timeStruct );
#elif defined( _WIN32 ) || defined( _WIN64 )
    (void) localtime_s( &timeStruct, &stamp );
#else
    /// MT-unsafe
    timeStruct = *std::localtime( &stamp );
#endif
    return timeStruct;
} // GetTimeStruct


bool GetTimeFromString( const LocString& time, const std::string& format, struct tm& timeStruct )
{
    LocStringStream str{ time };
    str >> std::get_time< LocChar >( &timeStruct, ToLocString( format ).c_str() );
    return str.fail();
} // GetTimeFromString


LocString GetTimeString( TimeType timestamp )
{
    uint16_t millis = 0;
    struct tm timeStruct = GetTimeStruct( timestamp, &millis );
    constexpr size_t bufSize = sizeof( "dd.mm.yyyy HH:MM:SS.000 TMZ" );

    LocString ft = GetFormatTime( timeStruct, "%d.%m.%Y %H:%M:%S.000 %Z", bufSize );
    /// Добавление милисекунд
    ft[ 20 ] = static_cast< LocChar >( ( ( millis % 1000 ) / 100 ) + LOCALIZED( '0' ) );
    ft[ 21 ] = static_cast< LocChar >( ( ( millis % 100 ) / 10 ) + LOCALIZED( '0' ) );
    ft[ 22 ] = static_cast< LocChar >( ( ( millis % 10 ) ) + LOCALIZED( '0' ) );
    return ft;
} // GetTimeString


LocString GetFormatTime( const struct tm& timeStruct,
    const std::string& format, size_t bufSize )
{
    if ( format.empty() )
    {
        return LOCALIZED( "" );
    }

    char* buffer = new char[ bufSize ]{ 0 };
    std::strftime( buffer, bufSize, format.c_str(), &timeStruct );
    LocString result = ToLocString( buffer );
    delete [] buffer;

    return result;
} // GetTimeString


bool StrCaseEq( const LocString& lhs, const LocString& rhs )
{
    return lhs.length() == rhs.length()
        && std::equal( lhs.cbegin(), lhs.cend(), rhs.cbegin(), LocCharCaseEq );
} // StrCaseEq


LocString Trim( const LocString& str, const LocString& trimChars )
{
    size_t leftPos = str.find_first_not_of( trimChars );
    if ( LocString::npos == leftPos )
    {
        return LOCALIZED( "" );
    }
    size_t rightPos = str.find_last_not_of( trimChars );
    return str.substr( leftPos, rightPos - leftPos );
} // Trim


std::vector< size_t > BitNumbers( uint64_t bits, size_t bitsCount, size_t offset )
{
    std::vector< size_t > bitNums;
    if ( bitsCount > sizeof( bits ) || sizeof( bits ) - bitsCount < offset )
    {
        return bitNums;
    }
    for ( size_t i = 0; i < bitsCount; ++i )
    {
        if( ( bits >> offset ) & ( 1ull << ( i % byteSize ) ) )
        {
            bitNums.push_back( i );
        }
    }
    return bitNums;
} // BitNumbers( uint64_t, size_t, size_t )


std::vector< size_t > BitNumbers( const std::vector< ByteType >& buffer, size_t bitsCount, size_t offset )
{
    std::vector< size_t > bitNums;
    const size_t bufSize = buffer.size() * byteSize;
    if ( bitsCount > bufSize || bufSize - bitsCount < offset )
    {
        return bitNums;
    }
    for ( size_t i = 0; i < bitsCount; ++i )
    {
        const size_t idx = i + offset;
        if( buffer.at( idx / byteSize ) & ( 1ull << ( idx % byteSize ) ) )
        {
            bitNums.push_back( idx );
        }
    }
    return bitNums;
} // BitNumbers( const std::vector< ByteType >&, size_t bitsCount, size_t offset )

} // namespace logika

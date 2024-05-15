/// @file Реализация загрузчика ресурсов
/// @copyright HypeRRu 2024

#include <logika/log/defines.h>
#include <logika/common/misc.h>

/// @cond
#include <fstream>
#include <iostream>
/// @endcond

namespace logika
{

namespace resources
{

template < typename Resource >
std::shared_ptr< Resource > Loader< Resource >::Load( const std::string& path )
{
    std::ifstream stream{ path, std::ios::in | std::ios::binary };
    if ( !stream.is_open() )
    {
        LOG_WRITE( LOG_ERROR, L"Unable to open file '" << ToLocString( path ) << L"'" );
        return nullptr;
    }
    std::shared_ptr< Resource > parser = std::make_shared< Resource >();
    if ( !parser->ParseFromIstream( &stream ) )
    {
        LOG_WRITE( LOG_ERROR, L"Unable to parse file '" << ToLocString( path ) << L"'" );
        return nullptr;
    }
    return parser;
} // Load

} // namespace resources

} // namespace logika

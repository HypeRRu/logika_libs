/// @file Реализация загрузчика ресурсов
/// @copyright HypeRRu 2024

#include <fstream>

#include <logika/log/defines.h>

namespace logika
{

namespace resources
{

template < typename Resource >
std::shared_ptr< Resource > Loader< Resource >::Load( const std::string& path )
{
    std::ifstream stream{ path };
    if ( !stream.is_open() )
    {
        LOG_WRITE( LOG_ERROR, "Unable to open file '" << path << "'" );
        return nullptr;
    }
    std::shared_ptr< Resource > parser = std::make_shared< Resource >();
    if ( !parser->ParseFromIstream( &stream ) )
    {
        LOG_WRITE( LOG_ERROR, "Unable to parse file '" << path << "'" );
        return nullptr;
    }
    return parser;
} // Load

} // namespace resources

} // namespace logika

/// @file Реализация функций для конвертации типов приборов
/// @copyright HypeRRu 2024

#include <logika/meters/utils/types_converter.h>

#include <unordered_map>
#include <stdexcept>

namespace logika
{

namespace meters
{

std::string DbTypeToString( DbType type )
{
    static const std::unordered_map< DbType, std::string > mapToString{
          { DbType::Byte,   "tinyint" }
        , { DbType::Int32,  "int" }
        , { DbType::Int64,  "bigint" }
        , { DbType::Single, "real" }
        , { DbType::Double, "float" }
        , { DbType::String, "varchar(128)" }
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    throw std::runtime_error{ "Unknown data type" };
} // DbTypeToString

} // namespace meters

} // namespace logika
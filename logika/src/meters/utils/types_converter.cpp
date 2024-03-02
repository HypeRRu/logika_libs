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
        /// @todo Массивы
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    /// @todo Стоит ли использовать exception?
    return "";
} // DbTypeToString


std::string TagKindToString( TagKind::Type type )
{
    static const std::unordered_map< TagKind::Type, std::string > mapToString{
          { TagKind::Parameter, "tuning" }
        , { TagKind::Info,      "informational" }
        , { TagKind::Realtime,  "current" }
        , { TagKind::TotalCtr,  "total" }
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return "?";
} // TagKindToString

} // namespace meters

} // namespace logika

/// @file Реализация функций для конвертации типов приборов
/// @copyright HypeRRu 2024

#include <logika/meters/utils/types_converter.h>

/// @cond
#include <unordered_map>
#include <stdexcept>
/// @endcond

namespace logika
{

namespace meters
{

LocString DbTypeToString( DbType type )
{
    static const std::unordered_map< DbType, LocString > mapToString{
          { DbType::Byte,   L"tinyint" }
        , { DbType::Int32,  L"int" }
        , { DbType::Int64,  L"bigint" }
        , { DbType::Single, L"real" }
        , { DbType::Double, L"float" }
        , { DbType::String, L"varchar(128)" }
        /// @todo Массивы
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    /// @todo Стоит ли использовать exception?
    return L"";
} // DbTypeToString


LocString TagKindToString( TagKind::Type type )
{
    static const std::unordered_map< TagKind::Type, LocString > mapToString{
          { TagKind::Parameter, L"tuning" }
        , { TagKind::Info,      L"informational" }
        , { TagKind::Realtime,  L"current" }
        , { TagKind::TotalCtr,  L"total" }
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return L"?";
} // TagKindToString

} // namespace meters

} // namespace logika

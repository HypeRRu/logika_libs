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
          { DbType::Byte,        LOCALIZED( "tinyint" ) }
        , { DbType::Int32,       LOCALIZED( "int" ) }
        , { DbType::Int64,       LOCALIZED( "bigint" ) }
        , { DbType::Single,      LOCALIZED( "real" ) }
        , { DbType::Double,      LOCALIZED( "float" ) }
        , { DbType::String,      LOCALIZED( "varchar(128)" ) }
        , { DbType::ByteArray,   LOCALIZED( "byte[]" ) }
        , { DbType::Int32Array,  LOCALIZED( "int32[]" ) }
        , { DbType::StringArray, LOCALIZED( "string[]" ) }
        , { DbType::ObjectArray, LOCALIZED( "object[]" ) }
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return LOCALIZED( "" );
} // DbTypeToString


LocString TagKindToString( TagKind::Type type )
{
    static const std::unordered_map< TagKind::Type, LocString > mapToString{
          { TagKind::Parameter, LOCALIZED( "tuning" ) }
        , { TagKind::Info,      LOCALIZED( "informational" ) }
        , { TagKind::Realtime,  LOCALIZED( "current" ) }
        , { TagKind::TotalCtr,  LOCALIZED( "total" ) }
    };
    auto converted = mapToString.find( type );
    if ( converted != mapToString.cend() )
    {
        return converted->second;
    }
    return LOCALIZED( "?" );
} // TagKindToString

} // namespace meters

} // namespace logika

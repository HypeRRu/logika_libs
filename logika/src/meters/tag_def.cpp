/// @file Реализация типа тэга
/// @copyright HypeRRu 2024

#include <logika/meters/tag_def.h>

#include <logika/meters/utils/types_converter.h>

namespace logika
{

namespace meters
{

TagDef::TagDef( const ChannelDef& cdef, const TagDefSettings& settings)
    : ItemDefBase(
          cdef
        , settings.ordinal
        , settings.name
        , settings.description
        , settings.type
    )
    , stdVar_{ settings.stdVar }
    , dbTypeStr_{
        !settings.dbType.empty() ? settings.dbType
        : DbTypeToString( settings.type ) 
    }
    , displayFormat_{ settings.displayFormat }
    , key_{ LOCALIZED( "" ) }
{} // TagDef


StdVar TagDef::GetStdVar() const
{
    return stdVar_;
} // GetStdVar


LocString TagDef::GetDbTypeStr() const
{
    return dbTypeStr_;
} // GetDbTypeStr


LocString TagDef::GetDisplayFormat() const
{
    return displayFormat_;
} // GetDisplayFormat


LocString TagDef::GetKey() const
{
    return LOCALIZED( "" );
} // GetKey

} // namespace meters

} // namespace logika

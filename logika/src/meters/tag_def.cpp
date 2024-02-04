/// @file Реализация типа тэга
/// @copyright HypeRRu 2024

#include <logika/meters/tag_def.h>

#include <logika/meters/utils/types_converter.h>

namespace logika
{

namespace meters
{

/// Базовое описание тэга

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
    , key_{ "" }
{} // TagDef


StdVar TagDef::GetStdVar() const
{
    return stdVar_;
} // GetStdVar


std::string TagDef::GetDbTypeStr() const
{
    return dbTypeStr_;
} // GetDbTypeStr


std::string TagDef::GetDisplayFormat() const
{
    return displayFormat_;
} // GetDisplayFormat


std::string TagDef::GetKey() const
{
    return "";
} // GetKey


/// Описание тэга с данными

DataTagDef::DataTagDef( const ChannelDef& cdef, const DataTagDefSettings& settings )
    : TagDef( cdef , settings )
    , kind_{ settings.kind }
    , descriptionEx_{ settings.descriptionEx }
    , range_{ settings.range }
    , isBasicParam_{ settings.isBasicParam }
    , updateRate_{ settings.updateRate }
{} // DataTagDef


TagKind::Type DataTagDef::GetKind() const
{
    return kind_;
} // GetKind


std::string DataTagDef::GetDescriptionEx() const
{
    return descriptionEx_;
} // GetDescriptionEx


std::string DataTagDef::GetRange() const
{
    return range_;
} // GetRange


bool DataTagDef::IsBasicParam() const
{
    return isBasicParam_;
} // IsBasicParam


uint32_t DataTagDef::GetUpdateRate() const
{
    return updateRate_;
} // GetUpdateRate


std::string DataTagDef::ToString() const
{
    return key_ + " " + std::to_string( ordinal_ ) + " " + name_;
} // ToString

} // namespace meters

} // namespace logika

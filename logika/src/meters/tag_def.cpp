/// @file Реализация типа тэга
/// @copyright HypeRRu 2024

#include <logika/meters/tag_def.h>

#include <logika/meters/utils/types_converter.h>

namespace logika
{

namespace meters
{

/// Базовое описание тэга

TagDef::TagDef( const ChannelDef& cdef, int ordinal, const std::string& name
    , StdVar stdVar, const std::string& description, DbType type
    , const std::string& dbType, const std::string& displayFormat )
    : ItemDefBase( cdef, ordinal, name, description, type )
    , stdVar_{ stdVar }
    , dbTypeStr_{ !dbType.empty() ? dbType : DbTypeToString( baseType_ ) }
    , displayFormat_{ displayFormat }
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

DataTagDef::DataTagDef( const ChannelDef& cdef, int ordinal, const std::string& name
    , StdVar stdVar, const std::string& description, DbType type
    , const std::string& dbType, const std::string& displayFormat
    , TagKind::Type kind, bool isBasic, const uint32_t updRate
    , const std::string& descEx, const std::string& range )
    : TagDef( cdef, ordinal, name, stdVar, description, type, dbType, displayFormat )
    , kind_{ kind }
    , descriptionEx_{ descEx }
    , range_{ range }
    , isBasicParam_{ isBasic }
    , updateRate_{ updRate }
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

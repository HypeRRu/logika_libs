/// @file Реализация типа тэга
/// @copyright HypeRRu 2024

#include <logika/meters/data_tag_def.h>

namespace logika
{

namespace meters
{

DataTagDef::DataTagDef( const ChannelDef& cdef, const DataTagDefSettings& settings )
    : TagDef( cdef , settings )
    , kind_{ settings.kind }
    , descriptionEx_{ settings.descriptionEx }
    , range_{ settings.range }
    , isBasicParam_{ settings.isBasicParam }
    , updateRate_{ settings.updateRate }
    , address_{ name_ }
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


int32_t DataTagDef::GetIndex() const
{
    return index_;
} // GetIndex


std::string DataTagDef::GetAddress() const
{
    return address_;
} // GetAddress


std::string DataTagDef::ToString() const
{
    return key_ + " " + std::to_string( ordinal_ ) + " " + name_;
} // ToString

} // namespace meters

} // namespace logika

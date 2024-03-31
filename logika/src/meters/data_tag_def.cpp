/// @file Реализация типа тэга
/// @copyright HypeRRu 2024

#include <logika/meters/data_tag_def.h>

#include <logika/common/misc.h>

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


LocString DataTagDef::GetDescriptionEx() const
{
    return descriptionEx_;
} // GetDescriptionEx


LocString DataTagDef::GetRange() const
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


LocString DataTagDef::GetAddress() const
{
    return address_;
} // GetAddress


LocString DataTagDef::ToString() const
{
    return key_ + L" " + ToLocString( std::to_string( ordinal_ ) ) + L" " + name_;
} // ToString

} // namespace meters

} // namespace logika

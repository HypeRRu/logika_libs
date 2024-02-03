/// @file Реализация типа базового элемента
/// @copyright HypeRRu 2024

#include <logika/meters/item_def_base.h>

namespace logika
{

namespace meters
{

ItemDefBase::ItemDefBase( const ChannelDef& cdef, int ordinal
    , const std::string& name, const std::string& description, DbType type )
    : channelDef_{ cdef }
    , ordinal_{ ordinal }
    , name_{ name }
    , description_{ description }
    , baseType_{ type }
{} // ItemDefBase


const ChannelDef& ItemDefBase::GetChannelDef() const
{
    return channelDef_;
} // GetChannelDef


const std::shared_ptr< Meter > ItemDefBase::GetMeter() const
{
    return channelDef_.meter;
} // GetMeter


int ItemDefBase::GetOrdinal() const
{
    return ordinal_;
} // GetOrdinal


const std::string& ItemDefBase::GetName() const
{
    return name_;
} // GetName


const std::string& ItemDefBase::GetDescription() const
{
    return description_;
} // GetDescription


DbType ItemDefBase::GetElementType() const
{
    return baseType_;
} // GetElementType
    

void ItemDefBase::SetName( const std::string& name )
{
    name_ = name;
} // SetName

} // namespace meters

} // namespace logika

/// @file Реализация типа базового элемента
/// @copyright HypeRRu 2024

#include <logika/meters/item_def_base.h>

namespace logika
{

namespace meters
{

ItemDefBase::ItemDefBase( const ChannelDef& cdef, int32_t ordinal
    , const LocString& name, const LocString& description, DbType type )
    : channelDef_{ cdef }
    , ordinal_{ ordinal }
    , name_{ name }
    , description_{ description }
    , baseType_{ type }
{} // ItemDefBase


void ItemDefBase::SetOrdinal( int32_t ordinal )
{
    ordinal_ = ordinal;
} // SetOrdinal


const ChannelDef& ItemDefBase::GetChannelDef() const
{
    return channelDef_;
} // GetChannelDef


const std::shared_ptr< IMeter > ItemDefBase::GetMeter() const
{
    return channelDef_.meter;
} // GetMeter


int ItemDefBase::GetOrdinal() const
{
    return ordinal_;
} // GetOrdinal


LocString ItemDefBase::GetName() const
{
    return name_;
} // GetName


LocString ItemDefBase::GetDescription() const
{
    return description_;
} // GetDescription


DbType ItemDefBase::GetElementType() const
{
    return baseType_;
} // GetElementType
    

void ItemDefBase::SetName( const LocString& name )
{
    name_ = name;
} // SetName

} // namespace meters

} // namespace logika

/// @file Реализация тэга
/// @copyright HypeRRu 2024

#include <logika/meters/tag.h>

#include <logika/common/misc.h>

/// @cond
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <iostream>
/// @endcond

namespace logika
{

namespace meters
{

Tag::Tag( const TagDef& def, int32_t channelNo )
    : def_{ def }
    , channel_{ def_.GetChannelDef(), channelNo }
{
    if (   channelNo <  channel_.start
        || channelNo >= channel_.start + channel_.count )
    {
        throw std::out_of_range{ "Channel No is invalid" };
    }
} // Tag


const TagDef& Tag::GetDef() const
{
    return def_;
} // GetDef


LocString Tag::GetName() const
{
    return def_.GetName();
} // GetName


LocString Tag::GetFieldName() const
{
    LocStringStream formatter;
    if ( channel_.no > 0 && !channel_.prefix.empty() )
    {
        formatter << channel_.prefix;
        formatter << std::setw( 2 ) << std::setfill( L'0' ) << channel_.no;
    }

    return ( !channel_.name.empty() ? ( channel_.name + L"_" ) : L"" )
        + def_.GetName() + formatter.str();
} // GetFieldName


int32_t Tag::GetOrdinal() const
{
    return def_.GetOrdinal();
} // GetOrdinal


LocString Tag::GetDescription() const
{
    return def_.GetDescription();
} // GetDescription


LocString Tag::GetAddress() const
{
    return address_;
} // GetAddress


LocString Tag::GetEu() const
{
    return eu_;
} // GetEu


void Tag::SetEu( const LocString& eu )
{
    eu_ = eu;
} // SetEu


LocString Tag::ToString() const
{
    return channel_.name + L"."
        + ToLocString( std::to_string( def_.GetOrdinal() ) )
        + L"(" + def_.GetName() + L")";
} // ToString

} // namespace meters

} // namespace logika

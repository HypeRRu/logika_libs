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

Tag::Tag( std::shared_ptr< TagDef > def, int32_t channelNo )
    : def_{ def }
    , channel_{
          def_ ? def_->GetChannelDef() : ChannelDef{ nullptr, L"", -1, 0, L"" }
        , channelNo
    }
{
    if (   channelNo <  channel_.start
        || channelNo >= channel_.start + static_cast< int32_t >( channel_.count ) )
    {
        throw std::out_of_range{ "Channel No is invalid" };
    }
} // Tag


std::shared_ptr< TagDef > Tag::GetDef() const
{
    return def_;
} // GetDef


LocString Tag::GetName() const
{
    return def_ ? def_->GetName() : L"";
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
        + GetName() + formatter.str();
} // GetFieldName


int32_t Tag::GetOrdinal() const
{
    return def_ ? def_->GetOrdinal() : -1;
} // GetOrdinal


LocString Tag::GetDescription() const
{
    return def_ ? def_->GetDescription() : L"";
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
        + ToLocString( std::to_string( GetOrdinal() ) )
        + L"(" + GetName() + L")";
} // ToString

} // namespace meters

} // namespace logika

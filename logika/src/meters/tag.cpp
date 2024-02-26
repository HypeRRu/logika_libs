/// @file Реализация тэга
/// @copyright HypeRRu 2024

#include <logika/meters/tag.h>

#include <stdexcept>
#include <iomanip>
#include <sstream>

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


std::string Tag::GetName() const
{
    return def_.GetName();
} // GetName


std::string Tag::GetFieldName() const
{
    std::stringstream formatter;
    if ( channel_.no > 0 && !channel_.prefix.empty() )
    {
        formatter << channel_.prefix;
        formatter << std::setw( 2 ) << std::setfill( '0' ) << channel_.no;
    }
    
    return ( !channel_.name.empty() ? ( channel_.name + "_" ) : "" )
        + def_.GetName() + formatter.str();
} // GetFieldName


int32_t Tag::GetOrdinal() const
{
    return def_.GetOrdinal();
} // GetOrdinal


std::string Tag::GetDescription() const
{
    return def_.GetDescription();
} // GetDescription


const std::string& Tag::GetAddress() const
{
    return address_;
} // GetAddress


const std::string& Tag::GetEu() const
{
    return eu_;
} // GetEu


void Tag::SetEu( const std::string& eu )
{
    eu_ = eu;
} // SetEu


std::string Tag::ToString() const
{
    return channel_.name + "." + std::to_string( def_.GetOrdinal() )
        + "(" + def_.GetName() + ")";
} // ToString

} // namespace meters

} // namespace logika

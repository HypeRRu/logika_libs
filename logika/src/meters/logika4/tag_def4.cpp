/// @file Реализация типа тэга Logika4
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/tag_def4.h>

#include <logika/meters/utils/types_converter.h>

namespace logika
{

namespace meters
{

TagDef4::TagDef4( const ChannelDef& cdef, const TagDef4Settings& settings )
    : DataTagDef( cdef, settings )
    , units_{ settings.units }
{
    key_ = name_;
} // TagDef4


std::string TagDef4::GetUnits() const
{
    return units_;
} // GetUnits


std::string TagDef4::ToString() const
{
    return std::to_string( ordinal_ ) + " " + channelDef_.prefix
        + " " + name_ + " " + TagKindToString( kind_ );
} // ToString

} // namespace meters

} // namespace logika

/// @file Реализация типа тэга Logika4
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/tag_def4.h>

#include <logika/meters/utils/types_converter.h>

namespace logika
{

namespace meters
{

DataTagDef4::DataTagDef4( const ChannelDef& cdef, const DataTagDef4Settings& settings )
    : DataTagDef( cdef, settings )
    , units_{ settings.units }
{
    key_ = name_;
} // DataTagDef4


std::string DataTagDef4::GetUnits() const
{
    return units_;
} // GetUnits


std::string DataTagDef4::ToString() const
{
    return std::to_string( ordinal_ ) + " " + channelDef_.prefix
        + " " + name_ + " " + TagKindToString( kind_ );
} // ToString

} // namespace meters

} // namespace logika

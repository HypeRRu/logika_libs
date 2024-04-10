/// @file Реализация типа тэга Logika4
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/tag_def4.h>

#include <logika/meters/utils/types_converter.h>
#include <logika/common/misc.h>

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


LocString TagDef4::GetUnits() const
{
    return units_;
} // GetUnits


LocString TagDef4::ToString() const
{
    return ToLocString( std::to_string( ordinal_ ) ) + LOCALIZED( " " ) + channelDef_.prefix
        + LOCALIZED( " " ) + name_ + LOCALIZED( " " ) + TagKindToString( kind_ );
} // ToString


CalcFieldDef::CalcFieldDef( const ChannelDef& cdef, const CalcFieldDefSettings& settings )
    : TagDef( cdef, settings )
    , channelNo_{ settings.channelNo }
    , insertAfter_{ settings.insertAfter }
    , expression_{ settings.expression }
    , eu_{ settings.eu }
{
    key_ = LOCALIZED( "" );
} // CalcFieldDef


int32_t CalcFieldDef::GetChannelNo() const
{
    return channelNo_;
} // GetChannelNo


LocString CalcFieldDef::GetInsertAfter() const
{
    return insertAfter_;
} // GetInsertAfter


LocString CalcFieldDef::GetExpression() const
{
    return expression_;
} // GetExpression


LocString CalcFieldDef::GetEu() const
{
    return eu_;
} // GetEu

} // namespace meters

} // namespace logika

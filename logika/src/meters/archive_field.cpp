/// @file Реализация поля архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive_field.h>

#include <logika/common/misc.h>

namespace logika
{

namespace meters
{

ArchiveField::ArchiveField( const ArchiveFieldDef& afdef, int32_t channelNo )
    : Tag( afdef, channelNo )
{
    address_ = afdef.GetAddress();
    archiveType_ = afdef.GetArchiveType();
} // ArchiveField


LocString ArchiveField::GetCaption() const
{
    return caption_;
} // GetCaption


void ArchiveField::SetCaption( const LocString& caption )
{
    caption_ = caption;
} // SetCaption


int32_t ArchiveField::GetArchiveOrdinal() const
{
    return archiveOrd_;
} // GetArchiveOrdinal


std::shared_ptr< ArchiveType > ArchiveField::GetArchiveType() const
{
    return archiveType_;
} // GetArchiveType


LocString ArchiveField::GetDisplayFormat() const
{
    return def_.GetDisplayFormat();
} // GetDisplayFormat


LocString ArchiveField::ToString() const
{
    const LocString channelNoStr = ToLocString( channel_.no ? std::to_string( channel_.no ) : "" );
    LocString euStr = Trim( eu_ );
    if ( !euStr.empty() )
    {
        euStr = L"[" + euStr + L"]";
    }
    return channel_.name + L" " + channelNoStr + L" " + euStr;
} // ToString

} // namespace meters

} // namespace logika

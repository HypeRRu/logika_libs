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


std::string ArchiveField::GetCaption() const
{
    return caption_;
} // GetCaption


void ArchiveField::SetCaption( const std::string& caption )
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


std::string ArchiveField::GetDisplayFormat() const
{
    return def_.GetDisplayFormat();
} // GetDisplayFormat


std::string ArchiveField::ToString() const
{
    const std::string channelNoStr = channel_.no ? std::to_string( channel_.no ) : "";
    std::string euStr = Trim( eu_ );
    if ( !euStr.empty() )
    {
        euStr = "[" + euStr + "]";
    }
    return channel_.name + " " + channelNoStr + " " + euStr;
} // ToString

} // namespace meters

} // namespace logika

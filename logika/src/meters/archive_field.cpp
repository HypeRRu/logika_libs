/// @file Реализация поля архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive_field.h>

#include <logika/common/misc.h>
#include <logika/meters/logika4/archive_field_def4.h>
#include <logika/meters/logika6/archive_field_def6.h>

namespace logika
{

namespace meters
{

ArchiveField::ArchiveField( const ArchiveFieldDef& afdef, int32_t channelNo )
    : Tag( afdef, channelNo )
{
    address_ = "";
    const ArchiveFieldDef4* afdef4 = dynamic_cast< const ArchiveFieldDef4* >( &afdef );
    const ArchiveFieldDef6* afdef6 = dynamic_cast< const ArchiveFieldDef6* >( &afdef );
    if ( afdef4 )
    {
        address_ = std::to_string( afdef4->GetOrdinal() );
    }
    else if ( afdef6 )
    {
        address_ = afdef6->GetAddress();
    }
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
    std::string euStr = Trim(eu_);
    if ( !euStr.empty() )
    {
        euStr = "[" + euStr + "]";
    }
    return channel_.name + " " + channelNoStr + " " + euStr;
} // ToString

} // namespace meters

} // namespace logika

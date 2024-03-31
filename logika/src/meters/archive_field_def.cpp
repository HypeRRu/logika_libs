/// @file Реализация описания поля архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive_field_def.h>

#include <logika/storage/storage.hpp>

namespace logika
{

namespace meters
{

ArchiveFieldDef::ArchiveFieldDef( const ChannelDef& cdef
    , const ArchiveFieldDefSettings& settings )
    : TagDef( cdef, settings )
    , archiveType_{ settings.archiveType }
    , address_{ "" }
{} // ArchiveFieldDef


std::shared_ptr< ArchiveType > ArchiveFieldDef::GetArchiveType() const
{
    return archiveType_;
} // ArchiveFieldDef


std::string ArchiveFieldDef::GetAddress() const
{
    return address_;
} // GetAddress


std::string ArchiveFieldDef::ToString() const
{
    return channelDef_.prefix + " " + name_;
} // ToString

} // namespace meters

} // namespace logika

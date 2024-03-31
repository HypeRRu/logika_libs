/// @file Реализация типа описания архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive_def.h>

namespace logika
{

namespace meters
{

ArchiveDef::ArchiveDef( const ChannelDef& cdef, const ArchiveDefSettings& settings )
    : ItemDefBase(
          cdef
        , settings.ordinal
        , settings.name
        , settings.description
        , settings.type )
    , archiveType_{ settings.archType }
    , capacity_{ settings.capacity }
{} // ArchiveDef


std::shared_ptr< ArchiveType > ArchiveDef::GetArchiveType() const
{
    return archiveType_;
} // GetArchiveType


uint32_t ArchiveDef::GetCapacity() const
{
    return capacity_;
} // GetCapacity


LocString ArchiveDef::ToString() const
{
    return ( archiveType_ ? archiveType_->ToString() : L"[unknown]" )
        + L" " + name_ + L" (" + description_ + L")";
} // ToString

} // namespace meters

} // namespace logika

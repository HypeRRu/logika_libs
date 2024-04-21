/// @file Реализация описания поля архива Logika4M
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/archive_field_def4m.h>

namespace logika
{

namespace meters
{

ArchiveFieldDef4M::ArchiveFieldDef4M( const ChannelDef& cdef
    , const ArchiveFieldDef4MSettings& settings )
    : ArchiveFieldDef4( cdef, settings )
    , fieldIndex_{ settings.fieldIndex }
{} // ArchiveFieldDef4M


uint32_t ArchiveFieldDef4M::GetFieldIndex() const
{
    return fieldIndex_;
} // GetFieldIndex

} // namespace meters

} // namespace logika

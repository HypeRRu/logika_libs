/// @file Реализация типа описания архива Logika4M
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/archive_def4m.h>

namespace logika
{

namespace meters
{

ArchiveDef4M::ArchiveDef4M( const ChannelDef& cdef, const ArchiveDef4MSettings& settings )
    : ArchiveDef( cdef, settings )
{
    ordinal_ = -1;
} // ArchiveDef4M

} // namespace meters

} // namespace logika

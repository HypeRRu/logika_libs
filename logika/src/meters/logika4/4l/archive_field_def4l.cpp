/// @file Реализация описания поля архива Logika4L
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/archive_field_def4l.h>

namespace logika
{

namespace meters
{

ArchiveFieldDef4L::ArchiveFieldDef4L( const ChannelDef& cdef
    , const ArchiveFieldDef4LSettings& settings )
    : ArchiveFieldDef4( cdef, settings )
    , internalType_{ settings.binType }
    , fieldOffset_{ settings.fieldOffset }
{} // ArchiveFieldDef4L


BinaryType4L::Type ArchiveFieldDef4L::GetInternalType() const
{
    return internalType_;
} // GetInternalType


MeterAddressType ArchiveFieldDef4L::GetFieldOffset() const
{
    return fieldOffset_;
} // GetFieldOffset

} // namespace meters

} // namespace logika

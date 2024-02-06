/// @file Реализация типа описания архива Logika4L
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/archive_def4l.h>

namespace logika
{

namespace meters
{

ArchiveDef4L::ArchiveDef4L( const ChannelDef& cdef, const ArchiveDef4LSettings& settings )
    : ArchiveDef( cdef, settings )
    , isTiny42_{ settings.isTiny42 }
    , recordSize_{ settings.recordSize }
    , indexAddr_{ settings.indexAddr }
    , headerAddr_{ settings.headerAddr }
    , recordsAddr_{ settings.recordsAddr }
    , indexAddr2_{ settings.indexAddr2 }
    , headerAddr2_{ settings.headerAddr2 }
    , recordsAddr2_{ settings.recordsAddr2 }
{} // ArchiveDef4L


bool ArchiveDef4L::IsTiny() const
{
    return isTiny42_;
} // IsTiny


uint32_t ArchiveDef4L::GetRecordSize() const
{
    return recordSize_;
} // GetRecordSize


MeterAddressType ArchiveDef4L::GetIndexAddr() const
{
    return indexAddr_;
} // GetIndexAddr


MeterAddressType ArchiveDef4L::GetHeaderAddr() const
{
    return headerAddr_;
} // GetHeaderAddr


MeterAddressType ArchiveDef4L::GetRecordsAddr() const
{
    return recordsAddr_;
} // GetRecordsAddr


MeterAddressType ArchiveDef4L::GetIndexAddr2() const
{
    return indexAddr2_;
} // GetIndexAddr2


MeterAddressType ArchiveDef4L::GetHeaderAddr2() const
{
    return headerAddr2_;
} // GetHeaderAddr2


MeterAddressType ArchiveDef4L::GetRecordsAddr2() const
{
    return recordsAddr2_;
} // GetRecordsAddr2

} // namespace meters

} // namespace logika

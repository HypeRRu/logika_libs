/// @file Реализация архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive.h>

namespace logika
{

namespace meters
{

/// Базовый тип архива

Archive::Archive( const Meter& meter, const ArchiveType& archiveType )
    : meter_{ meter }
    , archiveType_{ archiveType }
{} // Archive


const Meter& Archive::GetMeter() const
{
    return meter_;
} // GetMeter


const ArchiveType& Archive::GetArchiveType() const
{
    return archiveType_;
} // GetArchiveType

} // namespace meters

} // namespace logika

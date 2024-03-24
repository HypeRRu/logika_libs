/// @file Реализация архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive.h>

namespace logika
{

namespace meters
{

/// Базовый тип архива

Archive::Archive( const Meter& meter, std::shared_ptr< ArchiveType > archiveType )
    : meter_{ meter }
    , archiveType_{ archiveType }
{} // Archive


const Meter& Archive::GetMeter() const
{
    return meter_;
} // GetMeter


std::shared_ptr< ArchiveType > Archive::GetArchiveType() const
{
    return archiveType_;
} // GetArchiveType

} // namespace meters

} // namespace logika

#pragma once

#ifndef X6_ARCHIVE_FIELDS_H
#define X6_ARCHIVE_FIELDS_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace X6ArchiveFields
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace X6ArchiveFields

} // namespace logika_migration

#endif // X6_ARCHIVE_FIELDS_H

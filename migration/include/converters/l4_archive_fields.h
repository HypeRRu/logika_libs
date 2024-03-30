#pragma once

#ifndef L4_ARCHIVE_FIELDS_H
#define L4_ARCHIVE_FIELDS_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace L4ArchiveFields
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace L4ArchiveFields

} // namespace logika_migration

#endif // L4_ARCHIVE_FIELDS_H

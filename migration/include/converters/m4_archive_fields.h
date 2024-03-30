#pragma once

#ifndef M4_ARCHIVE_FIELDS_H
#define M4_ARCHIVE_FIELDS_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace M4ArchiveFields
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace M4ArchiveFields

} // namespace logika_migration

#endif // M4_ARCHIVE_FIELDS_H

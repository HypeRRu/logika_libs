#pragma once

#ifndef ARCHIVE_TYPES_H
#define ARCHIVE_TYPES_H

#include <string>

namespace logika_migration
{

namespace ArchiveTypes
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace ArchiveTypes

} // namespace logika_migration

#endif // ARCHIVE_TYPES_H

#pragma once

#ifndef X6_ARCHIVES_H
#define X6_ARCHIVES_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace X6Archives
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace X6Archives

} // namespace logika_migration

#endif // X6_ARCHIVES_H

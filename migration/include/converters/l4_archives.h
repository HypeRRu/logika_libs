#pragma once

#ifndef L4_ARCHIVES_H
#define L4_ARCHIVES_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace L4Archives
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace L4Archives

} // namespace logika_migration

#endif // L4_ARCHIVES_H

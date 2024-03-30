#pragma once

#ifndef L4_TYPES_H
#define L4_TYPES_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace L4Types
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace L4Types

} // namespace logika_migration

#endif // L4_TYPES_H

#pragma once

#ifndef BUSES_H
#define BUSES_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace Buses
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace Buses

} // namespace logika_migration

#endif // BUSES_H

#pragma once

#ifndef M4_TAGS_H
#define M4_TAGS_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace M4Tags
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace M4Tags

} // namespace logika_migration

#endif // M4_TAGS_H

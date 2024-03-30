#pragma once

#ifndef TAG_KINDS_H
#define TAG_KINDS_H

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace TagKinds
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace TagKinds

} // namespace logika_migration

#endif // TAG_KINDS_H

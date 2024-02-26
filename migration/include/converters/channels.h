#pragma once

#ifndef CHANNELS_H
#define CHANNELS_H

#include <string>

namespace logika_migration
{

namespace Channels
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace Channels

} // namespace logika_migration

#endif // CHANNELS_H

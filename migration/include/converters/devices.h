#pragma once

#ifndef DEVICES_H
#define DEVICES_H

#include <string>

namespace logika_migration
{

namespace Devices
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace Devices

} // namespace logika_migration

#endif // DEVICES_H

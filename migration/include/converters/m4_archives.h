#pragma once

#ifndef M4_ARCHIVES_H
#define M4_ARCHIVES_H

#include <string>

namespace logika_migration
{

namespace M4Archives
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace M4Archives

} // namespace logika_migration

#endif // M4_ARCHIVES_H

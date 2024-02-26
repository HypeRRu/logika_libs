#pragma once

#ifndef L4_TAGS_H
#define L4_TAGS_H

#include <string>

namespace logika_migration
{

namespace L4Tags
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace L4Tags

} // namespace logika_migration

#endif // L4_TAGS_H

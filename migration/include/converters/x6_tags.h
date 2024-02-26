#pragma once

#ifndef X6_TAGS_H
#define X6_TAGS_H

#include <string>

namespace logika_migration
{

namespace X6Tags
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace X6Tags

} // namespace logika_migration

#endif // X6_TAGS_H

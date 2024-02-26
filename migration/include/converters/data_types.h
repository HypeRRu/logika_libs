#pragma once

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>

namespace logika_migration
{

namespace DataTypes
{

bool Migrate( const std::string& inFileName, const std::string& outFileName );

} // namespace DataTypes

} // namespace logika_migration

#endif // DATA_TYPES_H

#pragma once

#ifndef ENUMS_STRING_TO_PB_H
#define ENUMS_STRING_TO_PB_H

#include <string>

#include <logika/resources/enums.pb.h>

namespace logika_migration
{

namespace EnumsStringToPb
{

logika::ArchiveTypeEnum ArchiveTypeFromString( const std::string& str );

logika::BusTypeEnum BusTypeFromString( const std::string& str );

logika::TagKindEnum TagKindFromString( const std::string& str );

logika::DataTypeEnum DataTypeFromString( const std::string& str );

logika::VarTypeEnum VarTypeFromString( const std::string& str );

logika::InternalTypeEnum InternalTypeFromString( const std::string& str );

logika::X6TagTypeEnum X6TagTypeFromString( const std::string& str );

} // namespace EnumsStringToPb

} // namespace logika_migration

#endif // ENUMS_STRING_TO_PB_H

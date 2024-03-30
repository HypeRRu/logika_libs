#pragma once

#ifndef ENUMS_STRING_TO_PB_H
#define ENUMS_STRING_TO_PB_H

#include <logika/resources/enums.pb.h>

/// @cond
#include <string>
/// @endcond

namespace logika_migration
{

namespace EnumsStringToPb
{

logika::resources::ArchiveTypeEnum ArchiveTypeFromString( const std::string& str );

logika::resources::BusTypeEnum BusTypeFromString( const std::string& str );

logika::resources::TagKindEnum TagKindFromString( const std::string& str );

logika::resources::DataTypeEnum DataTypeFromString( const std::string& str );

logika::resources::VarTypeEnum VarTypeFromString( const std::string& str );

logika::resources::InternalTypeEnum InternalTypeFromString( const std::string& str );

logika::resources::X6TagTypeEnum X6TagTypeFromString( const std::string& str );

logika::resources::MeasureKindEnum MeasureKindFromString( const std::string& str );

} // namespace EnumsStringToPb

} // namespace logika_migration

#endif // ENUMS_STRING_TO_PB_H

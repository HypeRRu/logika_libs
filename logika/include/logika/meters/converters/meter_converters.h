/// @file Определение конвертеров типов для работы с приборами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_METER_CONVERTERS_H
#define LOGIKA_METERS_CONVERTERS_METER_CONVERTERS_H

#include <logika/meters/types.h>

#include <logika/resources/archive_types.pb.h>

#include <memory>
#include <vector>

namespace logika
{

namespace meters
{

namespace converters
{

/// @brief Конвертер типа архива
class ArchiveTypeConverter
{
public:
    using ConvertedType     = std::shared_ptr< ArchiveType >;
    using ConvertedTypeList = std::vector< ConvertedType >;
    using FromType          = resources::ArchiveType;
    using FromTypeList      = resources::ArchiveTypes;

    static ConvertedType        Convert( const FromType& from );
    static ConvertedTypeList    Convert( const FromTypeList& fromList );

private:
    static std::string          ConvertName( const resources::ArchiveTypeEnum type );
    static LocString            ConvertAcronym( const resources::ArchiveTypeEnum type );
    static ArchiveTimingType    ConvertTimingType( const resources::ArchiveTypeEnum type );
    static TimeType             ConvertInterval( const resources::ArchiveTypeEnum type );
    static bool                 ConvertVariable( const resources::ArchiveTypeEnum type );

}; // class ArchiveTypeConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_METER_CONVERTERS_H

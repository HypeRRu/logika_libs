/// @file Определение конвертеров типов для работы с типами архивов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_ARCHIVE_TYPE_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_ARCHIVE_TYPE_CONVERTER_H

#include <logika/meters/converters/defs.h>

#include <logika/meters/types.h>

#include <logika/resources/archive_types.pb.h>

/// @cond
#include <memory>
#include <vector>
/// @endcond

namespace logika
{

namespace meters
{

namespace converters
{

/// @brief Конвертер типа архива
class LOGIKA_RESOURCE_CONVERTERS_EXPORT ArchiveTypeConverter
{
public:
    using ConvertedType     = std::shared_ptr< ArchiveType >;
    using ConvertedTypeList = std::vector< ConvertedType >;
    using FromType          = resources::ArchiveType;
    using FromTypeList      = resources::ArchiveTypes;

    static ConvertedType        Convert( const FromType& from );
    static ConvertedTypeList    Convert( const FromTypeList& fromList );

public:
    static LocString            ConvertName( const resources::ArchiveTypeEnum type );
    static LocString            ConvertAcronym( const resources::ArchiveTypeEnum type );
    static ArchiveTimingType    ConvertTimingType( const resources::ArchiveTypeEnum type );
    static TimeType             ConvertInterval( const resources::ArchiveTypeEnum type );
    static bool                 ConvertVariable( const resources::ArchiveTypeEnum type );

}; // class ArchiveTypeConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_ARCHIVE_TYPE_CONVERTER_H

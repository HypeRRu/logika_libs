/// @file Определение конвертеров типов для полей архивов Logika4M
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_M4_ARCHIVE_FIELD_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_M4_ARCHIVE_FIELD_CONVERTER_H

#include <logika/meters/converters/defs.h>

#include <logika/meters/types.h>
#include <logika/meters/channel.h>
#include <logika/meters/meter.h>
#include <logika/meters/logika4/4m/archive_def4m.h>
#include <logika/meters/logika4/4m/archive_field_def4m.h>

#include <logika/storage/storage.hpp>

#include <logika/resources/m4_archive_fields.pb.h>

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

/// @brief Конвертер полей архива Logika4M
class LOGIKA_RESOURCE_CONVERTERS_EXPORT M4ArchiveFieldConverter
{
public:
    using BaseConvertedType         = ArchiveFieldDef4M;
    using ConvertedType             = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList         = std::vector< ConvertedType >;
    using FromType                  = resources::M4ArchiveField;
    using FromTypeList              = resources::M4ArchiveFieldList;
    using MeterStorageType          = std::shared_ptr< storage::Storage< LocString, Meter > >;
    using ArchiveStorageType        = std::shared_ptr< storage::Storage< LocString, ArchiveDef4M > >;
    using ArchiveTypeStorageType    = std::shared_ptr< storage::Storage< LocString, ArchiveType > >;

    static ConvertedType        Convert( const FromType& from
        , MeterStorageType meterStorage, ArchiveStorageType archiveStorage
        , ArchiveTypeStorageType archiveTypeStorage );
    static ConvertedTypeList    Convert( const FromTypeList& fromList
        , MeterStorageType meterStorage, ArchiveStorageType archiveStorage
        , ArchiveTypeStorageType archiveTypeStorage );

public:
    static std::shared_ptr< Meter >         ConvertDevice(
        const LocString& devName, MeterStorageType meterStorage );
    static std::shared_ptr< ArchiveDef4M > ConvertArchive(
        const LocString& archiveLabel, ArchiveStorageType archiveStorage );
    static LocString                        ConvertArchiveTypeName(
        const resources::ArchiveTypeEnum type );
    static std::shared_ptr< ArchiveType >   ConvertArchiveType(
        const resources::ArchiveTypeEnum type, ArchiveTypeStorageType archiveTypeStorage );
    static DbType                           ConvertDbType( const resources::DataTypeEnum type );
    static StdVar                           ConvertVarType( const resources::VarTypeEnum type );

}; // class M4ArchiveFieldConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_M4_ARCHIVE_FIELD_CONVERTER_H

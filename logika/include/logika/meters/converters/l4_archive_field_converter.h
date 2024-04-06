/// @file Определение конвертеров типов для полей архивов Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_L4_ARCHIVE_FIELD_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_L4_ARCHIVE_FIELD_CONVERTER_H

#include <logika/meters/converters/defs.h>

#include <logika/meters/types.h>
#include <logika/meters/channel.h>
#include <logika/meters/meter.h>
#include <logika/meters/logika4/4l/archive_def4l.h>
#include <logika/meters/logika4/4l/archive_field_def4l.h>

#include <logika/storage/storage.hpp>

#include <logika/resources/l4_archive_fields.pb.h>

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

/// @brief Конвертер полей архива Logika4L
class LOGIKA_RESOURCE_CONVERTERS_EXPORT L4ArchiveFieldConverter
{
public:
    using BaseConvertedType         = ArchiveFieldDef4L;
    using ConvertedType             = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList         = std::vector< ConvertedType >;
    using FromType                  = resources::L4ArchiveField;
    using FromTypeList              = resources::L4ArchiveFieldList;
    using MeterStorageType          = std::shared_ptr< storage::Storage< LocString, Meter > >;
    using ArchiveStorageType        = std::shared_ptr< storage::Storage< LocString, ArchiveDef4L > >;
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
    static std::shared_ptr< ArchiveDef4L > ConvertArchive(
        const LocString& archiveLabel, ArchiveStorageType archiveStorage );
    static LocString                        ConvertArchiveTypeName(
        const resources::ArchiveTypeEnum type );
    static std::shared_ptr< ArchiveType >   ConvertArchiveType(
        const resources::ArchiveTypeEnum type, ArchiveTypeStorageType archiveTypeStorage );
    static DbType                           ConvertDbType( const resources::DataTypeEnum type );
    static StdVar                           ConvertVarType( const resources::VarTypeEnum type );
    static BinaryType4L::Type               ConvertInternalType( const resources::InternalTypeEnum type );

}; // class L4ArchiveFieldConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_L4_ARCHIVE_FIELD_CONVERTER_H

/// @file Определение конвертеров типов для архивов Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_L4_ARCHIVE_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_L4_ARCHIVE_CONVERTER_H

#include <logika/meters/types.h>
#include <logika/meters/channel.h>
#include <logika/meters/meter.h>
#include <logika/meters/logika4/4l/archive_def4l.h>

#include <logika/storage/storage.hpp>

#include <logika/resources/l4_archives.pb.h>

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

/// @brief Конвертер архива Logika4L
class L4ArchiveConverter
{
public:
    using BaseConvertedType         = ArchiveDef4L;
    using ConvertedType             = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList         = std::vector< ConvertedType >;
    using FromType                  = resources::L4Archive;
    using FromTypeList              = resources::L4ArchiveList;
    using MeterStorageType          = std::shared_ptr< storage::Storage< LocString, Meter > >;
    using ChannelStorageType        = std::shared_ptr< storage::Storage< LocString, ChannelDef > >;
    using ArchiveTypeStorageType    = std::shared_ptr< storage::Storage< LocString, ArchiveType > >;

    static ConvertedType        Convert( const FromType& from
        , MeterStorageType meterStorage, ChannelStorageType channelStorage
        , ArchiveTypeStorageType archiveTypeStorage );
    static ConvertedTypeList    Convert( const FromTypeList& fromList
        , MeterStorageType meterStorage, ChannelStorageType channelStorage
        , ArchiveTypeStorageType archiveTypeStorage );

public:
    static std::shared_ptr< Meter >         ConvertDevice(
        const LocString& devName, MeterStorageType meterStorage );
    static std::shared_ptr< ChannelDef >    ConvertChannel(
        const LocString& channelLabel, ChannelStorageType channelStorage );
    static std::shared_ptr< ArchiveType >   ConvertArchiveType(
        const resources::ArchiveTypeEnum type, ArchiveTypeStorageType archiveTypeStorage );
    static DbType                           ConvertDbType( const resources::DataTypeEnum type );

}; // class L4ArchiveConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_L4_ARCHIVE_CONVERTER_H

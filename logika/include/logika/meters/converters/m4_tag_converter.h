/// @file Определение конвертеров типов для тэгов Logika4M
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_M4_TAG_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_M4_TAG_CONVERTER_H

#include <logika/meters/converters/defs.h>

#include <logika/meters/types.h>
#include <logika/meters/channel.h>
#include <logika/meters/meter.h>
#include <logika/meters/logika4/4m/tag_def4m.h>

#include <logika/storage/storage.hpp>

#include <logika/resources/m4_tags.pb.h>

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

/// @brief Конвертер тэга Logika4M
class LOGIKA_RESOURCE_CONVERTERS_EXPORT M4TagConverter
{
public:
    using BaseConvertedType     = TagDef4M;
    using ConvertedType         = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList     = std::vector< ConvertedType >;
    using FromType              = resources::M4Tag;
    using FromTypeList          = resources::M4TagList;
    using MeterStorageType      = std::shared_ptr< storage::Storage< LocString, Meter > >;
    using ChannelStorageType    = std::shared_ptr< storage::Storage< LocString, ChannelDef > >;

    static ConvertedType        Convert( const FromType& from
        , MeterStorageType meterStorage, ChannelStorageType channelStorage );
    static ConvertedTypeList    Convert( const FromTypeList& fromList
        , MeterStorageType meterStorage, ChannelStorageType channelStorage );

public:
    static std::shared_ptr< Meter >         ConvertDevice(
        const LocString& devName, MeterStorageType meterStorage );
    static std::shared_ptr< ChannelDef >    ConvertChannel(
        const LocString& channelLabel, ChannelStorageType channelStorage );
    static TagKind::Type                    ConvertTagKind( const resources::TagKindEnum type );
    static DbType                           ConvertDbType( const resources::DataTypeEnum type );
    static StdVar                           ConvertVarType( const resources::VarTypeEnum type );

}; // class M4TagConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_M4_TAG_CONVERTER_H

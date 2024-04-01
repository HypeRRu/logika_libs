/// @file Определение конвертеров типов для тэгов Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_L4_TAG_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_L4_TAG_CONVERTER_H

#include <logika/meters/types.h>
#include <logika/meters/channel.h>
#include <logika/meters/meter.h>
#include <logika/meters/logika4/4l/tag_def4l.h>

#include <logika/storage/storage.hpp>

#include <logika/resources/l4_tags.pb.h>

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

/// @brief Конвертер тэга Logika4L
class L4TagConverter
{
public:
    using BaseConvertedType     = TagDef4L;
    using ConvertedType         = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList     = std::vector< ConvertedType >;
    using FromType              = resources::L4Tag;
    using FromTypeList          = resources::L4TagList;
    using MeterStorageType      = std::shared_ptr< storage::Storage< LocString, Meter > >;
    using ChannelStorageType    = std::shared_ptr< storage::Storage< LocString, ChannelDef > >;

    static ConvertedType        Convert( const FromType& from
        , MeterStorageType meterStorage, ChannelStorageType channelStorage );
    static ConvertedTypeList    Convert( const FromTypeList& fromList
        , MeterStorageType meterStorage, ChannelStorageType channelStorage );

public:
    /// @todo Использовать общий для всех конвертеров метод (не дублировать здесь)
    static std::shared_ptr< Meter >         ConvertDevice(
        const LocString& devName, MeterStorageType meterStorage );
    static std::shared_ptr< ChannelDef >    ConvertChannel(
        const LocString& channelLabel, ChannelStorageType channelStorage );
    static TagKind::Type                    ConvertTagKind( const resources::TagKindEnum type );
    static DbType                           ConvertDbType( const resources::DataTypeEnum type );
    static StdVar                           ConvertVarType( const resources::VarTypeEnum type );
    static BinaryType4L::Type               ConvertInternalType( const resources::InternalTypeEnum type );

}; // class L4TagConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_L4_TAG_CONVERTER_H

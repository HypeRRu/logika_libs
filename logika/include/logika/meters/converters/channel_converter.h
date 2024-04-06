/// @file Определение конвертеров типов для каналов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_CHANNEL_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_CHANNEL_CONVERTER_H

#include <logika/meters/converters/defs.h>

#include <logika/meters/types.h>
#include <logika/meters/channel.h>
#include <logika/meters/meter.h>

#include <logika/storage/storage.hpp>

#include <logika/resources/channels.pb.h>

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

/// @brief Конвертер типа прибора
class LOGIKA_RESOURCE_CONVERTERS_EXPORT ChannelConverter
{
public:
    using BaseConvertedType = ChannelDef;
    using ConvertedType     = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList = std::vector< ConvertedType >;
    using FromType          = resources::Channel;
    using FromTypeList      = resources::ChannelList;
    using StorageType       = std::shared_ptr< storage::Storage< LocString, Meter > >;

    static ConvertedType        Convert( const FromType& from, StorageType meterStorage );
    static ConvertedTypeList    Convert( const FromTypeList& fromList, StorageType meterStorage );

public:
    static std::shared_ptr< Meter > ConvertDevice( const LocString& devName, StorageType meterStorage );

}; // class ChannelConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_CHANNEL_CONVERTER_H

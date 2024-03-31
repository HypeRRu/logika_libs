/// @file Определение конвертеров типов для каналов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_CHANNEL_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_CHANNEL_CONVERTER_H

#include <logika/meters/types.h>
#include <logika/meters/channel.h>

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
class ChannelConverter
{
public:
    using BaseConvertedType = ChannelDef;
    using ConvertedType     = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList = std::vector< ConvertedType >;
    using FromType          = resources::Channel;
    using FromTypeList      = resources::ChannelList;

    static ConvertedType        Convert( const FromType& from );
    static ConvertedTypeList    Convert( const FromTypeList& fromList );

public:
    static std::shared_ptr< Meter > ConvertDevice( const LocString& devName );

}; // class ChannelConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_CHANNEL_CONVERTER_H

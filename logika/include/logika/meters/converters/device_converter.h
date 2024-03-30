/// @file Определение конвертеров типов для прибора
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_DEVICE_CONVERTER_H
#define LOGIKA_METERS_CONVERTERS_DEVICE_CONVERTER_H

#include <logika/meters/types.h>
#include <logika/meters/meter.h>

#include <logika/resources/devices.pb.h>

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
class DeviceConverter
{
public:
    using BaseConvertedType = Meter;
    using ConvertedType     = std::shared_ptr< BaseConvertedType >;
    using ConvertedTypeList = std::vector< ConvertedType >;
    using FromType          = resources::Device;
    using FromTypeList      = resources::DeviceList;

    static ConvertedType        Convert( const FromType& from );
    static ConvertedTypeList    Convert( const FromTypeList& fromList );

public:
    static MeasureKind      ConvertMeasureKind( const resources::MeasureKindEnum type );
    static BusProtocolType  ConvertBusType( const resources::BusTypeEnum type );

}; // class DeviceConverter

} // namespace converters

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CONVERTERS_DEVICE_CONVERTER_H

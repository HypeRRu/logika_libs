/// @file Реализация конвертеров типов для прибора
/// @copyright HypeRRu 2024

#include <logika/meters/converters/device_converter.h>

#include <logika/common/types.h>
#include <logika/common/misc.h>

/// @cond
#include <unordered_map>
/// @endcond

namespace logika
{

namespace meters
{

namespace converters
{

DeviceConverter::ConvertedType DeviceConverter::Convert( const DeviceConverter::FromType& from )
{
    /// @todo Данные поля игнорируются
    // from.m4();
    // from.has_auxno();
    // from.auxno();
    const MeasureKind mk        = DeviceConverter::ConvertMeasureKind( from.media() );
    const LocString desc        = ToLocString( from.description() );
    const uint32_t mc           = from.has_pipes() ? from.pipes() : 0; // maxChannels
    const uint32_t mg           = from.has_consumers() ? from.consumers() : 0; // maxGroups
    const BusProtocolType bt    = DeviceConverter::ConvertBusType( from.bus() );

    /// @todo Создание инстансов сразу нужных типов
    return Meter::Create< Meter >( mk, from.key(), desc, mc, mg, bt );
} // Convert( const DeviceConverter::FromType& from )


DeviceConverter::ConvertedTypeList DeviceConverter::Convert( const DeviceConverter::FromTypeList& fromList )
{
    ConvertedTypeList converted;
    if ( !fromList.list_size() )
    {
        return converted;
    }
    for ( auto from: fromList.list() )
    {
        converted.push_back( DeviceConverter::Convert( from ) );
    }
    return converted;
} // Convert( const DeviceConverter::FromTypeList& fromList )


MeasureKind DeviceConverter::ConvertMeasureKind( const resources::MeasureKindEnum type )
{
    static const std::unordered_map< resources::MeasureKindEnum, MeasureKind > converter{
          { logika::resources::MEASURE_KIND_T,  MeasureKind::T }
        , { logika::resources::MEASURE_KIND_G,  MeasureKind::G }
        , { logika::resources::MEASURE_KIND_E,  MeasureKind::E }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return MeasureKind::Undefined;
} // ConvertMeasureKind


BusProtocolType DeviceConverter::ConvertBusType( const resources::BusTypeEnum type )
{
    static const std::unordered_map< resources::BusTypeEnum, BusProtocolType > converter{
          { logika::resources::BUS_TYPE_RS_BUS,  BusProtocolType::RSBus }
        , { logika::resources::BUS_TYPE_SP_BUS,  BusProtocolType::SPBus }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return BusProtocolType::Undefined;
} // ConvertBusType

} // namespace converters

} // namespace meters

} // namespace logika

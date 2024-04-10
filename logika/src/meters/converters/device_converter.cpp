/// @file Реализация конвертеров типов для прибора
/// @copyright HypeRRu 2024

#include <logika/meters/converters/device_converter.h>

#include <logika/common/types.h>
#include <logika/common/misc.h>

#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/meters/logika4/4l/spg741.h>
#include <logika/meters/logika4/4l/spt941.h>
#include <logika/meters/logika4/4l/spt941_10.h>
#include <logika/meters/logika4/4l/spt942.h>
#include <logika/meters/logika4/4l/spt943.h>
#include <logika/meters/logika4/4m/logika4m.h>
#include <logika/meters/logika4/4m/lgk410.h>
#include <logika/meters/logika4/4m/spg740.h>
#include <logika/meters/logika4/4m/spg742.h>
#include <logika/meters/logika4/4m/spt940.h>
#include <logika/meters/logika4/4m/spt941_20.h>
#include <logika/meters/logika4/4m/spt943rev3.h>
#include <logika/meters/logika4/4m/spt944.h>

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
    const MeasureKind mk        = ConvertMeasureKind( from.media() );
    const LocString caption     = ToLocString( from.key() );
    const LocString desc        = ToLocString( from.description() );
    const uint32_t mc           = from.has_pipes() ? from.pipes() : 0; // maxChannels
    const uint32_t mg           = from.has_consumers() ? from.consumers() : 0; // maxGroups
    const BusProtocolType bt    = ConvertBusType( from.bus() );

    if ( caption == LOCALIZED( "LGK410" ) )
    {
        return Meter::Create< Lgk410 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPG740" ) )
    {
        return Meter::Create< Spg740 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPG741" ) )
    {
        return Meter::Create< Spg741 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPG742" ) )
    {
        return Meter::Create< Spg742 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT940" ) )
    {
        return Meter::Create< Spt940 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT941" ) )
    {
        return Meter::Create< Spt941 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT941_10" ) )
    {
        return Meter::Create< Spt941_10 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT941_20" ) )
    {
        return Meter::Create< Spt941_20 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT942" ) )
    {
        return Meter::Create< Spt942 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT943" ) )
    {
        return Meter::Create< Spt943 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT943rev3" ) )
    {
        return Meter::Create< Spt943rev3 >( mk, caption, desc, mc, mg, bt );
    }
    else if ( caption == LOCALIZED( "SPT944" ) )
    {
        return Meter::Create< Spt944 >( mk, caption, desc, mc, mg, bt );
    }
    /// @todo Создание инстансов сразу нужных типов (Logika6)
    return Meter::Create< Meter >( mk, caption, desc, mc, mg, bt );
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
        converted.push_back( Convert( from ) );
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

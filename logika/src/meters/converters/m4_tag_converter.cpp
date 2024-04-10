/// @file Реализация конвертеров типов для тэгов Logika4M
/// @copyright HypeRRu 2024

#include <logika/meters/converters/m4_tag_converter.h>

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

M4TagConverter::ConvertedType M4TagConverter::Convert( const M4TagConverter::FromType& from
    , M4TagConverter::MeterStorageType meterStorage, M4TagConverter::ChannelStorageType channelStorage )
{
    std::shared_ptr< Meter > meter = ConvertDevice( ToLocString( from.device() ), meterStorage );
    if ( !meter )
    {
        return nullptr;
    }
    LocString channelLabel = meter->GetCaption() + L"." + ToLocString( from.channel() );
    std::shared_ptr< ChannelDef > channelDef = ConvertChannel( channelLabel, channelStorage );
    if ( !channelDef )
    {
        return nullptr;
    }
    
    TagDef4MSettings settings;
    settings.dbType = ToLocString( from.dbtype() );
    settings.description = ToLocString( from.description() );
    settings.descriptionEx = from.has_descriptionex() ? ToLocString( from.descriptionex() ) : L"";
    settings.displayFormat = from.has_displayformat() ? ToLocString( from.displayformat() ) : L"";
    settings.isBasicParam = from.basic();
    settings.kind = ConvertTagKind( from.kind() );
    settings.name = ToLocString( from.name() );
    settings.ordinal = from.ordinal();
    settings.range = from.has_range() ? ToLocString( from.range() ) : L"";
    settings.stdVar = ConvertVarType( from.vartype() );
    settings.type = ConvertDbType( from.datatype() );
    settings.units = from.has_units() ? ToLocString( from.units() ) : L"";
    settings.updateRate = from.updaterate();

    return TagDef4M::Create< TagDef4M >( *channelDef, settings );
} // Convert( const FromType&, MeterStorageType, ChannelStorageType )


M4TagConverter::ConvertedTypeList M4TagConverter::Convert( const M4TagConverter::FromTypeList& fromList
    , M4TagConverter::MeterStorageType meterStorage, M4TagConverter::ChannelStorageType channelStorage )
{
    ConvertedTypeList converted;
    if ( !fromList.list_size() )
    {
        return converted;
    }
    for ( auto from: fromList.list() )
    {
        converted.push_back( Convert( from, meterStorage, channelStorage ) );
    }
    return converted;
} // Convert( const FromTypeList&, MeterStorageType, ChannelStorageType )


std::shared_ptr< Meter > M4TagConverter::ConvertDevice(
    const LocString& devName, M4TagConverter::MeterStorageType meterStorage )
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< Meter > device;
    meterStorage->GetItem( devName, device );
    return device;
} // ConvertDevice


std::shared_ptr< ChannelDef > M4TagConverter::ConvertChannel(
    const LocString& channelLabel, M4TagConverter::ChannelStorageType channelStorage )
{
    if ( !channelStorage )
    {
        return nullptr;
    }
    std::shared_ptr< ChannelDef > channelDef;
    channelStorage->GetItem( channelLabel, channelDef );
    return channelDef;
} // ConvertChannel


TagKind::Type M4TagConverter::ConvertTagKind( const resources::TagKindEnum type )
{
    static const std::unordered_map< resources::TagKindEnum, TagKind::Type > converter{
          { logika::resources::TAG_KIND_PARAMETER,  TagKind::Parameter }
        , { logika::resources::TAG_KIND_INFO,       TagKind::Info }
        , { logika::resources::TAG_KIND_REALTIME,   TagKind::Realtime }
        , { logika::resources::TAG_KIND_TOTAL_CTR,  TagKind::TotalCtr }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return TagKind::Undefined;
} // ConvertTagKind


DbType M4TagConverter::ConvertDbType( const resources::DataTypeEnum type )
{
    static const std::unordered_map< resources::DataTypeEnum, DbType > converter{
          { logika::resources::DATA_TYPE_BYTE,          DbType::Byte }
        , { logika::resources::DATA_TYPE_INT32,         DbType::Int32 }
        , { logika::resources::DATA_TYPE_INT64,         DbType::Int64 }
        , { logika::resources::DATA_TYPE_SINGLE,        DbType::Single }
        , { logika::resources::DATA_TYPE_DOUBLE,        DbType::Double }
        , { logika::resources::DATA_TYPE_STRING,        DbType::String }
        , { logika::resources::DATA_TYPE_BYTE_ARRAY,    DbType::ByteArray }
        , { logika::resources::DATA_TYPE_INT32_ARRAY,   DbType::Int32Array }
        , { logika::resources::DATA_TYPE_STRING_ARRAY,  DbType::StringArray }
        , { logika::resources::DATA_TYPE_OBJECT_ARRAY,  DbType::ObjectArray }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return DbType::Undefined;
} // ConvertDbType


StdVar M4TagConverter::ConvertVarType( const resources::VarTypeEnum type )
{
    static const std::unordered_map< resources::VarTypeEnum, StdVar > converter{
          { logika::resources::VAR_TYPE_SP,         StdVar::SP }
        , { logika::resources::VAR_TYPE_NS,         StdVar::NS }
        , { logika::resources::VAR_TYPE_AVG,        StdVar::AVG }
        , { logika::resources::VAR_TYPE_G,          StdVar::G }
        , { logika::resources::VAR_TYPE_M,          StdVar::M }
        , { logika::resources::VAR_TYPE_P,          StdVar::P }
        , { logika::resources::VAR_TYPE_DP,         StdVar::dP }
        , { logika::resources::VAR_TYPE_T,          StdVar::T }
        , { logika::resources::VAR_TYPE_TI,         StdVar::ti }
        , { logika::resources::VAR_TYPE_V,          StdVar::V }
        , { logika::resources::VAR_TYPE_W,          StdVar::W }
        , { logika::resources::VAR_TYPE_AUX_INT,    StdVar::auxInt }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return StdVar::Undefined;
} // ConvertVarType

} // namespace converters

} // namespace meters

} // namespace logika

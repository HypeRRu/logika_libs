/// @file Реализация конвертеров типов для тэгов Logika4L
/// @copyright HypeRRu 2024

#include <logika/meters/converters/l4_tag_converter.h>

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

L4TagConverter::ConvertedType L4TagConverter::Convert( const L4TagConverter::FromType& from
    , L4TagConverter::MeterStorageType meterStorage, L4TagConverter::ChannelStorageType channelStorage )
{
    std::shared_ptr< Meter > meter = L4TagConverter::ConvertDevice(
        ToLocString( from.device() ), meterStorage );
    if ( !meter )
    {
        return nullptr;
    }
    LocString channelLabel = meter->GetCaption() + L"_" + ToLocString( from.channel() );
    std::shared_ptr< ChannelDef > channelDef = L4TagConverter::ConvertChannel( channelLabel, channelStorage );
    if ( !channelDef )
    {
        return nullptr;
    }
    
    TagDef4LSettings settings;
    settings.addonAddress = from.has_addon() ? from.addon() : 0x00;
    settings.address = from.has_address() ? from.address() : 0x00;
    settings.cAddonOffset = from.has_addonoffset() ? from.addonoffset() : 0x00;
    settings.cOffset = from.has_channeloffset() ? from.channeloffset() : 0x00;
    settings.dbType = ToLocString( from.dbtype() );
    settings.description = ToLocString( from.description() );
    settings.descriptionEx = from.has_descriptionex() ? ToLocString( from.descriptionex() ) : L"";
    settings.displayFormat = from.has_displayformat() ? ToLocString( from.displayformat() ) : L"";
    settings.inRam = from.inram();
    settings.intType = ConvertInternalType( from.internaltype() );
    settings.isBasicParam = from.basic();
    settings.kind = ConvertTagKind( from.kind() );
    settings.name = ToLocString( from.name() );
    settings.ordinal = from.ordinal();
    settings.range = from.has_range() ? ToLocString( from.range() ) : L"";
    settings.stdVar = ConvertVarType( from.vartype() );
    settings.type = ConvertDbType( from.datatype() );
    settings.units = from.has_units() ? ToLocString( from.units() ) : L"";
    settings.updateRate = from.updaterate();

    return TagDef4L::Create< TagDef4L >(
          *channelDef
        , settings
    );
} // Convert( const FromType&, MeterStorageType, ChannelStorageType )


L4TagConverter::ConvertedTypeList L4TagConverter::Convert( const L4TagConverter::FromTypeList& fromList
    , L4TagConverter::MeterStorageType meterStorage, L4TagConverter::ChannelStorageType channelStorage )
{
    ConvertedTypeList converted;
    if ( !fromList.list_size() )
    {
        return converted;
    }
    for ( auto from: fromList.list() )
    {
        converted.push_back( L4TagConverter::Convert( from, meterStorage, channelStorage ) );
    }
    return converted;
} // Convert( const FromTypeList&, MeterStorageType, ChannelStorageType )


std::shared_ptr< Meter > L4TagConverter::ConvertDevice(
    const LocString& devName, L4TagConverter::MeterStorageType meterStorage )
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< Meter > device;
    meterStorage->GetItem( devName, device );
    return device;
} // ConvertDevice


std::shared_ptr< ChannelDef > L4TagConverter::ConvertChannel(
    const LocString& channelLabel, L4TagConverter::ChannelStorageType channelStorage )
{
    if ( !channelStorage )
    {
        return nullptr;
    }
    std::shared_ptr< ChannelDef > channelDef;
    channelStorage->GetItem( channelLabel, channelDef );
    return channelDef;
} // ConvertChannel


TagKind::Type L4TagConverter::ConvertTagKind( const resources::TagKindEnum type )
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


DbType L4TagConverter::ConvertDbType( const resources::DataTypeEnum type )
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


StdVar L4TagConverter::ConvertVarType( const resources::VarTypeEnum type )
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


BinaryType4L::Type L4TagConverter::ConvertInternalType( const resources::InternalTypeEnum type )
{
    static const std::unordered_map< resources::InternalTypeEnum, BinaryType4L::Type > converter{
          { logika::resources::INTERNAL_TYPE_R32,               BinaryType4L::R32 }
        , { logika::resources::INTERNAL_TYPE_TRIPLE_R32,        BinaryType4L::TripleR32 }
        , { logika::resources::INTERNAL_TYPE_TIME,              BinaryType4L::Time }
        , { logika::resources::INTERNAL_TYPE_DATE,              BinaryType4L::Date }
        , { logika::resources::INTERNAL_TYPE_MM_DD_DATE,        BinaryType4L::MmDdDate }
        , { logika::resources::INTERNAL_TYPE_BIT_ARRAY32,       BinaryType4L::BitArray32 }
        , { logika::resources::INTERNAL_TYPE_BIT_ARRAY24,       BinaryType4L::BitArray24 }
        , { logika::resources::INTERNAL_TYPE_BIT_ARRAY16,       BinaryType4L::BitArray16 }
        , { logika::resources::INTERNAL_TYPE_BIT_ARRAY8,        BinaryType4L::BitArray8 }
        , { logika::resources::INTERNAL_TYPE_DB_ENTRY,          BinaryType4L::DbEntry }
        , { logika::resources::INTERNAL_TYPE_DB_ENTRY_BYTE,     BinaryType4L::DbEntryByte }
        , { logika::resources::INTERNAL_TYPE_U8,                BinaryType4L::U8 }
        , { logika::resources::INTERNAL_TYPE_I32R32,            BinaryType4L::I32R32 }
        , { logika::resources::INTERNAL_TYPE_MM_HH_TIME,        BinaryType4L::MmHhTime }
        , { logika::resources::INTERNAL_TYPE_NS_RECORD,         BinaryType4L::NsRecord }
        , { logika::resources::INTERNAL_TYPE_IZM_RECORD,        BinaryType4L::IzmRecord }
        , { logika::resources::INTERNAL_TYPE_ARCHIVE_STRUCT,    BinaryType4L::ArchiveStruct }
        , { logika::resources::INTERNAL_TYPE_MODEL_CHAR,        BinaryType4L::ModelChar }
        , { logika::resources::INTERNAL_TYPE_U24,               BinaryType4L::U24 }
        , { logika::resources::INTERNAL_TYPE_SERVICE_RECORD_TS, BinaryType4L::ServiceRecordTimestamp }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return BinaryType4L::Undefined;
} // ConvertInternalType

} // namespace converters

} // namespace meters

} // namespace logika

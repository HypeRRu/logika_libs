/// @file Реализация конвертеров типов для архивов Logika4M
/// @copyright HypeRRu 2024

#include <logika/meters/converters/l4_archive_converter.h>

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

L4ArchiveConverter::ConvertedType L4ArchiveConverter::Convert( const L4ArchiveConverter::FromType& from
    , L4ArchiveConverter::MeterStorageType meterStorage, L4ArchiveConverter::ChannelStorageType channelStorage
    , L4ArchiveConverter::ArchiveTypeStorageType archiveTypeStorage )
{
    std::shared_ptr< Meter > meter = ConvertDevice( ToLocString( from.device() ), meterStorage );
    if ( !meter )
    {
        return nullptr;
    }
    std::shared_ptr< ArchiveType > archiveType = ConvertArchiveType( from.archivetype(), archiveTypeStorage );
    if ( !archiveType )
    {
        return nullptr;
    }
    LocString channelLabel = meter->GetCaption() + L"_" + ToLocString( from.channel() );
    std::shared_ptr< ChannelDef > channelDef = ConvertChannel( channelLabel, channelStorage );
    if ( !channelDef )
    {
        return nullptr;
    }

    ArchiveDef4LSettings settings;
    settings.archType       = archiveType;
    settings.capacity       = from.count();
    settings.description    = ToLocString( from.description() );
    settings.headerAddr     = from.has_headersaddr() ? from.headersaddr() : 0x0;
    settings.headerAddr2    = from.has_headersaddr2() ? from.headersaddr2() : 0x0;
    settings.indexAddr      = from.indexaddr();
    settings.indexAddr2     = from.has_indexaddr2() ? from.indexaddr2() : 0x0;
    settings.isTiny42       = false;
    settings.name           = ToLocString( from.name() );
    settings.ordinal        = -1;
    settings.recordsAddr    = from.recordsaddr();
    settings.recordsAddr2   = from.has_recordsaddr2() ? from.recordsaddr2() : 0x0;
    settings.recordSize     = from.recordsize();
    settings.type           = ConvertDbType( from.recordtype() );
    
    return ArchiveDef4L::Create< ArchiveDef4L >( *channelDef, settings );
} // Convert( const FromType&, MeterStorageType, ChannelStorageType, ArchiveTypeStorageType )


L4ArchiveConverter::ConvertedTypeList L4ArchiveConverter::Convert( const L4ArchiveConverter::FromTypeList& fromList
    , L4ArchiveConverter::MeterStorageType meterStorage, L4ArchiveConverter::ChannelStorageType channelStorage
    , L4ArchiveConverter::ArchiveTypeStorageType archiveTypeStorage )
{
    ConvertedTypeList converted;
    if ( !fromList.list_size() )
    {
        return converted;
    }
    for ( auto from: fromList.list() )
    {
        converted.push_back( Convert( from, meterStorage, channelStorage, archiveTypeStorage ) );
    }
    return converted;
} // Convert( const FromTypeList&, MeterStorageType, ChannelStorageType, ArchiveTypeStorageType )


std::shared_ptr< Meter > L4ArchiveConverter::ConvertDevice(
    const LocString& devName, L4ArchiveConverter::MeterStorageType meterStorage )
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< Meter > device;
    meterStorage->GetItem( devName, device );
    return device;
} // ConvertDevice


std::shared_ptr< ChannelDef > L4ArchiveConverter::ConvertChannel(
    const LocString& channelLabel, L4ArchiveConverter::ChannelStorageType channelStorage )
{
    if ( !channelStorage )
    {
        return nullptr;
    }
    std::shared_ptr< ChannelDef > channelDef;
    channelStorage->GetItem( channelLabel, channelDef );
    return channelDef;
} // ConvertChannel


std::shared_ptr< ArchiveType > L4ArchiveConverter::ConvertArchiveType(
    const resources::ArchiveTypeEnum type, L4ArchiveConverter::ArchiveTypeStorageType archiveTypeStorage )
{
    if ( !archiveTypeStorage )
    {
        return nullptr;
    }
    
    static const std::unordered_map< resources::ArchiveTypeEnum, LocString > converter{
          { logika::resources::ARCHIVE_TYPE_CONTROL,    L"Control" }
        , { logika::resources::ARCHIVE_TYPE_DAY,        L"Day" }
        , { logika::resources::ARCHIVE_TYPE_DECADE,     L"Decade" }
        , { logika::resources::ARCHIVE_TYPE_DIAGS_LOG,  L"DiagsLog" }
        , { logika::resources::ARCHIVE_TYPE_ERRORS_LOG, L"ErrorsLog" }
        , { logika::resources::ARCHIVE_TYPE_HOUR,       L"Hour" }
        , { logika::resources::ARCHIVE_TYPE_MONTH,      L"Month" }
        , { logika::resources::ARCHIVE_TYPE_PARAMS_LOG, L"ParamsLog" }
        , { logika::resources::ARCHIVE_TYPE_POWER_LOG,  L"PowerLog" }
        , { logika::resources::ARCHIVE_TYPE_TURN,       L"Turn" }
        , { logika::resources::ARCHIVE_TYPE_MINUTE,     L"Minute" }
        , { logika::resources::ARCHIVE_TYPE_HALF_HOUR,  L"HalfHour" }
    };

    LocString typeName = L"";
    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        typeName = iter->second;
    }

    std::shared_ptr< ArchiveType > archiveType;
    archiveTypeStorage->GetItem( typeName, archiveType );
    return archiveType;
} // ConvertArchiveType


DbType L4ArchiveConverter::ConvertDbType( const resources::DataTypeEnum type )
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

} // namespace converters

} // namespace meters

} // namespace logika

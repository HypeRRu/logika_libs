/// @file Реализация конвертеров типов для архивов Logika4M
/// @copyright HypeRRu 2024

#include <logika/meters/converters/m4_archive_converter.h>

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

M4ArchiveConverter::ConvertedType M4ArchiveConverter::Convert( const M4ArchiveConverter::FromType& from
    , M4ArchiveConverter::MeterStorageType meterStorage, M4ArchiveConverter::ChannelStorageType channelStorage
    , M4ArchiveConverter::ArchiveTypeStorageType archiveTypeStorage )
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
    LocString channelLabel = meter->GetCaption() + LOCALIZED( "." ) + ToLocString( from.channel() );
    std::shared_ptr< ChannelDef > channelDef = ConvertChannel( channelLabel, channelStorage );
    if ( !channelDef )
    {
        return nullptr;
    }

    ArchiveDef4MSettings settings;
    settings.archType       = archiveType;
    settings.capacity       = from.capacity();
    settings.description    = ToLocString( from.description() );
    settings.name           = ToLocString( from.name() );
    /// @bug Как задавать
    settings.ordinal        = 0;
    settings.type           = ConvertDbType( from.recordtype() );
    
    return ArchiveDef4M::Create< ArchiveDef4M >( *channelDef, settings );
} // Convert( const FromType&, MeterStorageType, ChannelStorageType, ArchiveTypeStorageType )


M4ArchiveConverter::ConvertedTypeList M4ArchiveConverter::Convert( const M4ArchiveConverter::FromTypeList& fromList
    , M4ArchiveConverter::MeterStorageType meterStorage, M4ArchiveConverter::ChannelStorageType channelStorage
    , M4ArchiveConverter::ArchiveTypeStorageType archiveTypeStorage )
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


std::shared_ptr< Meter > M4ArchiveConverter::ConvertDevice(
    const LocString& devName, M4ArchiveConverter::MeterStorageType meterStorage )
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< Meter > device;
    meterStorage->GetItem( devName, device );
    return device;
} // ConvertDevice


std::shared_ptr< ChannelDef > M4ArchiveConverter::ConvertChannel(
    const LocString& channelLabel, M4ArchiveConverter::ChannelStorageType channelStorage )
{
    if ( !channelStorage )
    {
        return nullptr;
    }
    std::shared_ptr< ChannelDef > channelDef;
    channelStorage->GetItem( channelLabel, channelDef );
    return channelDef;
} // ConvertChannel


std::shared_ptr< ArchiveType > M4ArchiveConverter::ConvertArchiveType(
    const resources::ArchiveTypeEnum type, M4ArchiveConverter::ArchiveTypeStorageType archiveTypeStorage )
{
    if ( !archiveTypeStorage )
    {
        return nullptr;
    }
    
    static const std::unordered_map< resources::ArchiveTypeEnum, LocString > converter{
          { logika::resources::ARCHIVE_TYPE_CONTROL,    LOCALIZED( "Control" ) }
        , { logika::resources::ARCHIVE_TYPE_DAY,        LOCALIZED( "Day" ) }
        , { logika::resources::ARCHIVE_TYPE_DECADE,     LOCALIZED( "Decade" ) }
        , { logika::resources::ARCHIVE_TYPE_DIAGS_LOG,  LOCALIZED( "DiagsLog" ) }
        , { logika::resources::ARCHIVE_TYPE_ERRORS_LOG, LOCALIZED( "ErrorsLog" ) }
        , { logika::resources::ARCHIVE_TYPE_HOUR,       LOCALIZED( "Hour" ) }
        , { logika::resources::ARCHIVE_TYPE_MONTH,      LOCALIZED( "Month" ) }
        , { logika::resources::ARCHIVE_TYPE_PARAMS_LOG, LOCALIZED( "ParamsLog" ) }
        , { logika::resources::ARCHIVE_TYPE_POWER_LOG,  LOCALIZED( "PowerLog" ) }
        , { logika::resources::ARCHIVE_TYPE_TURN,       LOCALIZED( "Turn" ) }
        , { logika::resources::ARCHIVE_TYPE_MINUTE,     LOCALIZED( "Minute" ) }
        , { logika::resources::ARCHIVE_TYPE_HALF_HOUR,  LOCALIZED( "HalfHour" ) }
    };

    LocString typeName = LOCALIZED( "" );
    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        typeName = iter->second;
    }

    std::shared_ptr< ArchiveType > archiveType;
    archiveTypeStorage->GetItem( typeName, archiveType );
    return archiveType;
} // ConvertArchiveType


DbType M4ArchiveConverter::ConvertDbType( const resources::DataTypeEnum type )
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

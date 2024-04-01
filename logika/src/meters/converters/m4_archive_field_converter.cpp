/// @file Реализация конвертеров типов для полей архивов Logika4M
/// @copyright HypeRRu 2024

#include <logika/meters/converters/m4_archive_field_converter.h>

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

M4ArchiveFieldConverter::ConvertedType M4ArchiveFieldConverter::Convert( const M4ArchiveFieldConverter::FromType& from
    , M4ArchiveFieldConverter::MeterStorageType meterStorage, M4ArchiveFieldConverter::ArchiveStorageType archiveStorage
    , M4ArchiveFieldConverter::ArchiveTypeStorageType archiveTypeStorage )
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
    LocString archiveLabel = ToLocString( from.device() ) + L"_" + ConvertArchiveTypeName( from.archivetype() );
    std::shared_ptr< ArchiveDef4M > archive = ConvertArchive( archiveLabel, archiveStorage );
    if ( !archive )
    {
        return nullptr;
    }
    ArchiveFieldDef4MSettings settings;
    settings.archive        = archive;
    settings.archiveType    = archiveType;
    settings.dbType         = from.has_dbtype() ? ToLocString( from.dbtype() ) : L"";
    settings.description    = ToLocString( from.description() );
    settings.displayFormat  = ToLocString( from.displayformat() );
    settings.fieldIndex     = from.index();
    settings.name           = ToLocString( from.name() );
    /// @bug Как заполнять
    settings.ordinal        = 0;
    settings.stdVar         = ConvertVarType( from.vartype() );
    settings.type           = ConvertDbType( from.datatype() );
    settings.units          = from.has_units() ? ToLocString( from.units() ) : L"";

    return ArchiveFieldDef4M::Create< ArchiveFieldDef4M >( archive->GetChannelDef(), settings );
} // Convert( const FromType&, MeterStorageType, ArchiveStorageType, ArchiveTypeStorageType )


M4ArchiveFieldConverter::ConvertedTypeList M4ArchiveFieldConverter::Convert( const M4ArchiveFieldConverter::FromTypeList& fromList
    , M4ArchiveFieldConverter::MeterStorageType meterStorage, M4ArchiveFieldConverter::ArchiveStorageType archiveStorage
    , M4ArchiveFieldConverter::ArchiveTypeStorageType archiveTypeStorage )
{
    ConvertedTypeList converted;
    if ( !fromList.list_size() )
    {
        return converted;
    }
    for ( auto from: fromList.list() )
    {
        converted.push_back( Convert( from, meterStorage, archiveStorage, archiveTypeStorage ) );
    }
    return converted;
} // Convert( const FromTypeList&, MeterStorageType, ArchiveStorageType, ArchiveTypeStorageType )


std::shared_ptr< Meter > M4ArchiveFieldConverter::ConvertDevice(
    const LocString& devName, M4ArchiveFieldConverter::MeterStorageType meterStorage )
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< Meter > device;
    meterStorage->GetItem( devName, device );
    return device;
} // ConvertDevice


std::shared_ptr< ArchiveDef4M > M4ArchiveFieldConverter::ConvertArchive(
    const LocString& archiveLabel, M4ArchiveFieldConverter::ArchiveStorageType archiveStorage )
{
    if ( !archiveStorage )
    {
        return nullptr;
    }
    std::shared_ptr< ArchiveDef4M > archiveDef;
    archiveStorage->GetItem( archiveLabel, archiveDef );
    return archiveDef;
} // ConvertArchive


LocString M4ArchiveFieldConverter::ConvertArchiveTypeName(
    const resources::ArchiveTypeEnum type )
{
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

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return L"?";
} // ConvertArchiveTypeName


std::shared_ptr< ArchiveType > M4ArchiveFieldConverter::ConvertArchiveType(
    const resources::ArchiveTypeEnum type, M4ArchiveFieldConverter::ArchiveTypeStorageType archiveTypeStorage )
{
    if ( !archiveTypeStorage )
    {
        return nullptr;
    }

    LocString typeName = ConvertArchiveTypeName( type );

    std::shared_ptr< ArchiveType > archiveType;
    archiveTypeStorage->GetItem( typeName, archiveType );
    return archiveType;
} // ConvertArchiveType


DbType M4ArchiveFieldConverter::ConvertDbType( const resources::DataTypeEnum type )
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


StdVar M4ArchiveFieldConverter::ConvertVarType( const resources::VarTypeEnum type )
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

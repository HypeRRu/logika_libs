/// @file Реализация конвертеров типов для полей архивов Logika4L
/// @copyright HypeRRu 2024

#include <logika/meters/converters/l4_archive_field_converter.h>

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

L4ArchiveFieldConverter::ConvertedType L4ArchiveFieldConverter::Convert( const L4ArchiveFieldConverter::FromType& from
    , L4ArchiveFieldConverter::MeterStorageType meterStorage, L4ArchiveFieldConverter::ArchiveStorageType archiveStorage
    , L4ArchiveFieldConverter::ArchiveTypeStorageType archiveTypeStorage )
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
    std::shared_ptr< ArchiveDef4L > archive = ConvertArchive( archiveLabel, archiveStorage );
    if ( !archive )
    {
        return nullptr;
    }
    ArchiveFieldDef4LSettings settings;
    settings.archive        = archive;
    settings.archiveType    = archiveType;
    settings.binType        = ConvertInternalType( from.internaltype() );
    settings.dbType         = from.has_dbtype() ? ToLocString( from.dbtype() ) : L"";
    settings.description    = ToLocString( from.description() );
    settings.displayFormat  = L""; ///< Не используется
    settings.fieldOffset    = from.fieldoffset();
    settings.name           = ToLocString( from.name() );
    settings.stdVar         = ConvertVarType( from.vartype() );
    settings.type           = ConvertDbType( from.datatype() );
    settings.units          = from.has_units() ? ToLocString( from.units() ) : L"";

    return ArchiveFieldDef4L::Create< ArchiveFieldDef4L >( archive->GetChannelDef(), settings );
} // Convert( const FromType&, MeterStorageType, ArchiveStorageType, ArchiveTypeStorageType )


L4ArchiveFieldConverter::ConvertedTypeList L4ArchiveFieldConverter::Convert( const L4ArchiveFieldConverter::FromTypeList& fromList
    , L4ArchiveFieldConverter::MeterStorageType meterStorage, L4ArchiveFieldConverter::ArchiveStorageType archiveStorage
    , L4ArchiveFieldConverter::ArchiveTypeStorageType archiveTypeStorage )
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


std::shared_ptr< Meter > L4ArchiveFieldConverter::ConvertDevice(
    const LocString& devName, L4ArchiveFieldConverter::MeterStorageType meterStorage )
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< Meter > device;
    meterStorage->GetItem( devName, device );
    return device;
} // ConvertDevice


std::shared_ptr< ArchiveDef4L > L4ArchiveFieldConverter::ConvertArchive(
    const LocString& archiveLabel, L4ArchiveFieldConverter::ArchiveStorageType archiveStorage )
{
    if ( !archiveStorage )
    {
        return nullptr;
    }
    std::shared_ptr< ArchiveDef4L > archiveDef;
    archiveStorage->GetItem( archiveLabel, archiveDef );
    return archiveDef;
} // ConvertArchive


LocString L4ArchiveFieldConverter::ConvertArchiveTypeName(
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


std::shared_ptr< ArchiveType > L4ArchiveFieldConverter::ConvertArchiveType(
    const resources::ArchiveTypeEnum type, L4ArchiveFieldConverter::ArchiveTypeStorageType archiveTypeStorage )
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


DbType L4ArchiveFieldConverter::ConvertDbType( const resources::DataTypeEnum type )
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


StdVar L4ArchiveFieldConverter::ConvertVarType( const resources::VarTypeEnum type )
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


BinaryType4L::Type L4ArchiveFieldConverter::ConvertInternalType( const resources::InternalTypeEnum type )
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

#include "utils/enums_string_to_pb.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>

namespace logika_migration
{

namespace EnumsStringToPb
{

logika::resources::ArchiveTypeEnum ArchiveTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::resources::ArchiveTypeEnum > fromTo{
          { "control",      logika::resources::ARCHIVE_TYPE_CONTROL }
        , { "day",          logika::resources::ARCHIVE_TYPE_DAY }
        , { "decade",       logika::resources::ARCHIVE_TYPE_DECADE }
        , { "diagslog",     logika::resources::ARCHIVE_TYPE_DIAGS_LOG }
        , { "errorslog",    logika::resources::ARCHIVE_TYPE_ERRORS_LOG }
        , { "hour",         logika::resources::ARCHIVE_TYPE_HOUR }
        , { "month",        logika::resources::ARCHIVE_TYPE_MONTH }
        , { "paramslog",    logika::resources::ARCHIVE_TYPE_PARAMS_LOG }
        , { "powerlog",     logika::resources::ARCHIVE_TYPE_POWER_LOG }
        , { "turn",         logika::resources::ARCHIVE_TYPE_TURN }
        , { "minute",       logika::resources::ARCHIVE_TYPE_MINUTE }
        , { "halfhour",     logika::resources::ARCHIVE_TYPE_HALF_HOUR }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::resources::ARCHIVE_TYPE_UNDEFINED;
    }
    return iter->second;
} // ArchiveTypeFromString


logika::resources::BusTypeEnum BusTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::resources::BusTypeEnum > fromTo{
          { "rsbus",    logika::resources::BUS_TYPE_RS_BUS }
        , { "spbus",    logika::resources::BUS_TYPE_SP_BUS }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::resources::BUS_TYPE_UNDEFINED;
    }
    return iter->second;
} // BusTypeFromString


logika::resources::TagKindEnum TagKindFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::resources::TagKindEnum > fromTo{
          { "parameter",    logika::resources::TAG_KIND_PARAMETER }
        , { "info",         logika::resources::TAG_KIND_INFO }
        , { "realtime",     logika::resources::TAG_KIND_REALTIME }
        , { "totalctr",     logika::resources::TAG_KIND_TOTAL_CTR }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::resources::TAG_KIND_UNDEFINED;
    }
    return iter->second;
} // TagKindFromString


logika::resources::DataTypeEnum DataTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::resources::DataTypeEnum > fromTo{
          { "byte",     logika::resources::DATA_TYPE_BYTE }
        , { "int32",    logika::resources::DATA_TYPE_INT32 }
        , { "int64",    logika::resources::DATA_TYPE_INT64 }
        , { "single",   logika::resources::DATA_TYPE_SINGLE }
        , { "double",   logika::resources::DATA_TYPE_DOUBLE }
        , { "string",   logika::resources::DATA_TYPE_STRING }
        , { "byte[]",   logika::resources::DATA_TYPE_BYTE_ARRAY }
        , { "int32[]",  logika::resources::DATA_TYPE_INT32_ARRAY }
        , { "string[]", logika::resources::DATA_TYPE_STRING_ARRAY }
        , { "object[]", logika::resources::DATA_TYPE_OBJECT_ARRAY }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::resources::DATA_TYPE_UNDEFINED;
    }
    return iter->second;
} // DataTypeFromString


logika::resources::VarTypeEnum VarTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::resources::VarTypeEnum > fromTo{
          { "sp",       logika::resources::VAR_TYPE_SP }
        , { "ns",       logika::resources::VAR_TYPE_NS }
        , { "avg",      logika::resources::VAR_TYPE_AVG }
        , { "g",        logika::resources::VAR_TYPE_G }
        , { "m",        logika::resources::VAR_TYPE_M }
        , { "p",        logika::resources::VAR_TYPE_P }
        , { "dp",       logika::resources::VAR_TYPE_DP }
        , { "t",        logika::resources::VAR_TYPE_T }
        , { "ti",       logika::resources::VAR_TYPE_TI }
        , { "v",        logika::resources::VAR_TYPE_V }
        , { "w",        logika::resources::VAR_TYPE_W }
        , { "auxint",   logika::resources::VAR_TYPE_AUX_INT }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::resources::VAR_TYPE_UNDEFINED;
    }
    return iter->second;
} // VarTypeFromString


logika::resources::InternalTypeEnum InternalTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::resources::InternalTypeEnum > fromTo{
          { "r32",                  logika::resources::INTERNAL_TYPE_R32 }
        , { "r32x3",                logika::resources::INTERNAL_TYPE_TRIPLE_R32 }
        , { "time",                 logika::resources::INTERNAL_TYPE_TIME }
        , { "date",                 logika::resources::INTERNAL_TYPE_DATE }
        , { "mmdd",                 logika::resources::INTERNAL_TYPE_MM_DD_DATE }
        , { "bitarray32",           logika::resources::INTERNAL_TYPE_BIT_ARRAY32 }
        , { "bitarray24",           logika::resources::INTERNAL_TYPE_BIT_ARRAY24 }
        , { "bitarray16",           logika::resources::INTERNAL_TYPE_BIT_ARRAY16 }
        , { "bitarray8",            logika::resources::INTERNAL_TYPE_BIT_ARRAY8 }
        , { "dbentry",              logika::resources::INTERNAL_TYPE_DB_ENTRY }
        , { "dbentry_byte",         logika::resources::INTERNAL_TYPE_DB_ENTRY_BYTE }
        , { "u8",                   logika::resources::INTERNAL_TYPE_U8 }
        , { "i32r32",               logika::resources::INTERNAL_TYPE_I32R32 }
        , { "mmhh",                 logika::resources::INTERNAL_TYPE_MM_HH_TIME }
        , { "nsrecord",             logika::resources::INTERNAL_TYPE_NS_RECORD }
        , { "izmrecord",            logika::resources::INTERNAL_TYPE_IZM_RECORD }
        , { "archivestruct",        logika::resources::INTERNAL_TYPE_ARCHIVE_STRUCT }
        , { "modelchar",            logika::resources::INTERNAL_TYPE_MODEL_CHAR }
        , { "u24",                  logika::resources::INTERNAL_TYPE_U24 }
        , { "svcRecordTimestamp",   logika::resources::INTERNAL_TYPE_SERVICE_RECORD_TS }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::resources::INTERNAL_TYPE_UNDEFINED;
    }
    return iter->second;
} // InternalTypeFromString


logika::resources::X6TagTypeEnum X6TagTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::resources::X6TagTypeEnum > fromTo{
          { "array",        logika::resources::X6_TAG_TYPE_ARRAY }
        , { "structure",    logika::resources::X6_TAG_TYPE_STRUCTURE }
        , { "tag",          logika::resources::X6_TAG_TYPE_TAG }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::resources::X6_TAG_TYPE_UNDEFINED;
    }
    return iter->second;
} // X6TagTypeFromString

} // namespace EnumsStringToPb

} // namespace logika_migration

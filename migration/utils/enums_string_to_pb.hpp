#pragma once

#ifndef ENUMS_STRING_TO_PB_HPP
#define ENUMS_STRING_TO_PB_HPP

#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

#include "enums.pb.h"

namespace logika_migration
{

namespace EnumsStringToPb
{

logika::ArchiveTypeEnum ArchiveTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::ArchiveTypeEnum > fromTo{
          { "control",      logika::ARCHIVE_TYPE_CONTROL }
        , { "day",          logika::ARCHIVE_TYPE_DAY }
        , { "decade",       logika::ARCHIVE_TYPE_DECADE }
        , { "diagslog",     logika::ARCHIVE_TYPE_DIAGS_LOG }
        , { "errorslog",    logika::ARCHIVE_TYPE_ERRORS_LOG }
        , { "hour",         logika::ARCHIVE_TYPE_HOUR }
        , { "month",        logika::ARCHIVE_TYPE_MONTH }
        , { "paramslog",    logika::ARCHIVE_TYPE_PARAMS_LOG }
        , { "powerlog",     logika::ARCHIVE_TYPE_POWER_LOG }
        , { "turn",         logika::ARCHIVE_TYPE_TURN }
        , { "minute",       logika::ARCHIVE_TYPE_MINUTE }
        , { "halfhour",     logika::ARCHIVE_TYPE_HALF_HOUR }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::ARCHIVE_TYPE_UNDEFINED;
    }
    return iter->second;
} // ArchiveTypeFromString


logika::BusTypeEnum BusTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::BusTypeEnum > fromTo{
          { "rsbus",    logika::BUS_TYPE_RS_BUS }
        , { "spbus",    logika::BUS_TYPE_SP_BUS }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::BUS_TYPE_UNDEFINED;
    }
    return iter->second;
} // BusTypeFromString


logika::TagKindEnum TagKindFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::TagKindEnum > fromTo{
          { "parameter",    logika::TAG_KIND_PARAMETER }
        , { "info",         logika::TAG_KIND_INFO }
        , { "realtime",     logika::TAG_KIND_REALTIME }
        , { "totalctr",     logika::TAG_KIND_TOTAL_CTR }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::TAG_KIND_UNDEFINED;
    }
    return iter->second;
} // TagKindFromString


logika::DataTypeEnum DataTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::DataTypeEnum > fromTo{
          { "byte",     logika::DATA_TYPE_BYTE }
        , { "int32",    logika::DATA_TYPE_INT32 }
        , { "int64",    logika::DATA_TYPE_INT64 }
        , { "single",   logika::DATA_TYPE_SINGLE }
        , { "double",   logika::DATA_TYPE_DOUBLE }
        , { "string",   logika::DATA_TYPE_STRING }
        , { "byte[]",   logika::DATA_TYPE_BYTE_ARRAY }
        , { "int32[]",  logika::DATA_TYPE_INT32_ARRAY }
        , { "string[]", logika::DATA_TYPE_STRING_ARRAY }
        , { "object[]", logika::DATA_TYPE_OBJECT_ARRAY }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::DATA_TYPE_UNDEFINED;
    }
    return iter->second;
} // DataTypeFromString


logika::VarTypeEnum VarTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::VarTypeEnum > fromTo{
          { "sp",       logika::VAR_TYPE_SP }
        , { "ns",       logika::VAR_TYPE_NS }
        , { "avg",      logika::VAR_TYPE_AVG }
        , { "g",        logika::VAR_TYPE_G }
        , { "m",        logika::VAR_TYPE_M }
        , { "p",        logika::VAR_TYPE_P }
        , { "dp",       logika::VAR_TYPE_DP }
        , { "t",        logika::VAR_TYPE_T }
        , { "ti",       logika::VAR_TYPE_TI }
        , { "v",        logika::VAR_TYPE_V }
        , { "w",        logika::VAR_TYPE_W }
        , { "auxint",   logika::VAR_TYPE_AUX_INT }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::VAR_TYPE_UNDEFINED;
    }
    return iter->second;
} // VarTypeFromString


logika::InternalTypeEnum InternalTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::InternalTypeEnum > fromTo{
          { "r32",                  logika::INTERNAL_TYPE_R32 }
        , { "r32x3",                logika::INTERNAL_TYPE_TRIPLE_R32 }
        , { "time",                 logika::INTERNAL_TYPE_TIME }
        , { "date",                 logika::INTERNAL_TYPE_DATE }
        , { "mmdd",                 logika::INTERNAL_TYPE_MM_DD_DATE }
        , { "bitarray32",           logika::INTERNAL_TYPE_BIT_ARRAY32 }
        , { "bitarray24",           logika::INTERNAL_TYPE_BIT_ARRAY24 }
        , { "bitarray16",           logika::INTERNAL_TYPE_BIT_ARRAY16 }
        , { "bitarray8",            logika::INTERNAL_TYPE_BIT_ARRAY8 }
        , { "dbentry",              logika::INTERNAL_TYPE_DB_ENTRY }
        , { "dbentry_byte",         logika::INTERNAL_TYPE_DB_ENTRY_BYTE }
        , { "u8",                   logika::INTERNAL_TYPE_U8 }
        , { "i32r32",               logika::INTERNAL_TYPE_I32R32 }
        , { "mmhh",                 logika::INTERNAL_TYPE_MM_HH_TIME }
        , { "nsrecord",             logika::INTERNAL_TYPE_NS_RECORD }
        , { "izmrecord",            logika::INTERNAL_TYPE_IZM_RECORD }
        , { "archivestruct",        logika::INTERNAL_TYPE_ARCHIVE_STRUCT }
        , { "modelchar",            logika::INTERNAL_TYPE_MODEL_CHAR }
        , { "u24",                  logika::INTERNAL_TYPE_U24 }
        , { "svcRecordTimestamp",   logika::INTERNAL_TYPE_SERVICE_RECORD_TS }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::INTERNAL_TYPE_UNDEFINED;
    }
    return iter->second;
} // InternalTypeFromString


logika::X6TagTypeEnum X6TagTypeFromString( const std::string& str )
{
    static const std::unordered_map< std::string, logika::X6TagTypeEnum > fromTo{
          { "array",        logika::X6_TAG_TYPE_ARRAY }
        , { "structure",    logika::X6_TAG_TYPE_STRUCTURE }
        , { "tag",          logika::X6_TAG_TYPE_TAG }
    };

    std::string lowerStr( str.size(), ' ' );
    std::transform( str.begin(), str.end(), lowerStr.begin(), ::tolower );

    auto iter = fromTo.find( lowerStr );
    if ( fromTo.cend() == iter )
    {
        std::cerr << "'" << str << "'(" << lowerStr << ") not found\n";
        return logika::X6_TAG_TYPE_UNDEFINED;
    }
    return iter->second;
} // X6TagTypeFromString


} // namespace EnumsStringToPb

} // namespace logika_migration

#endif // ENUMS_STRING_TO_PB_HPP

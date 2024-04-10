/// @file Реализация конвертеров типов для работы с приборами
/// @copyright HypeRRu 2024

#include <logika/meters/converters/archive_type_converter.h>

#include <logika/common/types.h>
#include <logika/common/misc.h>

/// @cond
#include <chrono>
#include <unordered_map>
/// @endcond

/// @todo Вынести в файл
namespace // anonymous
{

using namespace std::chrono;

constexpr logika::TimeType tsZero   = 0;
constexpr logika::TimeType tsMinute = duration_cast< milliseconds >( minutes( 1 ) ).count();
constexpr logika::TimeType tsHour   = duration_cast< milliseconds >( hours( 1 ) ).count();
constexpr logika::TimeType tsDay    = 24 * tsHour;
constexpr logika::TimeType tsDecade = 10 * tsDay;
constexpr logika::TimeType tsMonth  = 30 * tsDay;

} // anonymous namespace


namespace logika
{

namespace meters
{

namespace converters
{

ArchiveTypeConverter::ConvertedType ArchiveTypeConverter::Convert( const ArchiveTypeConverter::FromType& from )
{
    return ArchiveType::Create< ArchiveType >(
          ConvertName( from.type() )
        , ToLocString( from.description() )
        , ConvertTimingType( from.type() )
        , ConvertAcronym( from.type() )
        , ConvertInterval( from.type() )
        , ConvertVariable( from.type() )
    );
} // Convert( const ArchiveTypeConverter::FromType& from )


ArchiveTypeConverter::ConvertedTypeList ArchiveTypeConverter::Convert( const ArchiveTypeConverter::FromTypeList& fromList )
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
} // Convert( const ArchiveTypeConverter::FromTypeList& fromList )


LocString ArchiveTypeConverter::ConvertName( const resources::ArchiveTypeEnum type )
{
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

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return LOCALIZED( "?" );
} // ConvertName


LocString ArchiveTypeConverter::ConvertAcronym( const resources::ArchiveTypeEnum type )
{
    static const std::unordered_map< resources::ArchiveTypeEnum, LocString > converter{
          { logika::resources::ARCHIVE_TYPE_CONTROL,    LOCALIZED( "Контр" ) }
        , { logika::resources::ARCHIVE_TYPE_DAY,        LOCALIZED( "Сут" )   }
        , { logika::resources::ARCHIVE_TYPE_DECADE,     LOCALIZED( "Дек" )   }
        , { logika::resources::ARCHIVE_TYPE_DIAGS_LOG,  LOCALIZED( "ДСа" )   }
        , { logika::resources::ARCHIVE_TYPE_ERRORS_LOG, LOCALIZED( "НСа" )   }
        , { logika::resources::ARCHIVE_TYPE_HOUR,       LOCALIZED( "Час" )   }
        , { logika::resources::ARCHIVE_TYPE_MONTH,      LOCALIZED( "Мес" )   }
        , { logika::resources::ARCHIVE_TYPE_PARAMS_LOG, LOCALIZED( "Изм" )   }
        , { logika::resources::ARCHIVE_TYPE_POWER_LOG,  LOCALIZED( "Пит" )   }
        , { logika::resources::ARCHIVE_TYPE_TURN,       LOCALIZED( "См" )    }
        , { logika::resources::ARCHIVE_TYPE_MINUTE,     LOCALIZED( "Мин" )   }
        , { logika::resources::ARCHIVE_TYPE_HALF_HOUR,  LOCALIZED( "ПЧас" )  }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return LOCALIZED( "?" );
} // ConvertAcronym


ArchiveTimingType ArchiveTypeConverter::ConvertTimingType( const resources::ArchiveTypeEnum type )
{
    static const std::unordered_map< resources::ArchiveTypeEnum, ArchiveTimingType > converter{
          { logika::resources::ARCHIVE_TYPE_CONTROL,    ArchiveTimingType::Synchronous  }
        , { logika::resources::ARCHIVE_TYPE_DAY,        ArchiveTimingType::Synchronous  }
        , { logika::resources::ARCHIVE_TYPE_DECADE,     ArchiveTimingType::Synchronous  }
        , { logika::resources::ARCHIVE_TYPE_DIAGS_LOG,  ArchiveTimingType::Asynchronous }
        , { logika::resources::ARCHIVE_TYPE_ERRORS_LOG, ArchiveTimingType::Asynchronous }
        , { logika::resources::ARCHIVE_TYPE_HOUR,       ArchiveTimingType::Synchronous  }
        , { logika::resources::ARCHIVE_TYPE_MONTH,      ArchiveTimingType::Synchronous  }
        , { logika::resources::ARCHIVE_TYPE_PARAMS_LOG, ArchiveTimingType::Asynchronous }
        , { logika::resources::ARCHIVE_TYPE_POWER_LOG,  ArchiveTimingType::Asynchronous }
        , { logika::resources::ARCHIVE_TYPE_TURN,       ArchiveTimingType::Asynchronous }
        , { logika::resources::ARCHIVE_TYPE_MINUTE,     ArchiveTimingType::Synchronous  }
        , { logika::resources::ARCHIVE_TYPE_HALF_HOUR,  ArchiveTimingType::Synchronous  }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return ArchiveTimingType::Undefined;
} // ConvertTimingType


TimeType ArchiveTypeConverter::ConvertInterval( const resources::ArchiveTypeEnum type )
{
    static const std::unordered_map< resources::ArchiveTypeEnum, TimeType > converter{
          { logika::resources::ARCHIVE_TYPE_CONTROL,    tsDay    }
        , { logika::resources::ARCHIVE_TYPE_DAY,        tsDay    }
        , { logika::resources::ARCHIVE_TYPE_DECADE,     tsDecade }
        , { logika::resources::ARCHIVE_TYPE_DIAGS_LOG,  tsZero   }
        , { logika::resources::ARCHIVE_TYPE_ERRORS_LOG, tsZero   }
        , { logika::resources::ARCHIVE_TYPE_HOUR,       tsHour   }
        , { logika::resources::ARCHIVE_TYPE_MONTH,      tsMonth  }
        , { logika::resources::ARCHIVE_TYPE_PARAMS_LOG, tsZero   }
        , { logika::resources::ARCHIVE_TYPE_POWER_LOG,  tsZero   }
        , { logika::resources::ARCHIVE_TYPE_TURN,       tsZero   }
        , { logika::resources::ARCHIVE_TYPE_MINUTE,     tsZero   }
        , { logika::resources::ARCHIVE_TYPE_HALF_HOUR,  tsZero   }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return tsZero;
} // ConvertInterval


bool ArchiveTypeConverter::ConvertVariable( const resources::ArchiveTypeEnum type )
{
    static const std::unordered_map< resources::ArchiveTypeEnum, bool > converter{
          { logika::resources::ARCHIVE_TYPE_CONTROL,    false }
        , { logika::resources::ARCHIVE_TYPE_DAY,        false }
        , { logika::resources::ARCHIVE_TYPE_DECADE,     false }
        , { logika::resources::ARCHIVE_TYPE_DIAGS_LOG,  false }
        , { logika::resources::ARCHIVE_TYPE_ERRORS_LOG, false }
        , { logika::resources::ARCHIVE_TYPE_HOUR,       false }
        , { logika::resources::ARCHIVE_TYPE_MONTH,      false }
        , { logika::resources::ARCHIVE_TYPE_PARAMS_LOG, false }
        , { logika::resources::ARCHIVE_TYPE_POWER_LOG,  false }
        , { logika::resources::ARCHIVE_TYPE_TURN,       false }
        , { logika::resources::ARCHIVE_TYPE_MINUTE,     true  }
        , { logika::resources::ARCHIVE_TYPE_HALF_HOUR,  true  }
    };

    auto iter = converter.find( type );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return false;
} // ConvertVariable

} // namespace converters

} // namespace meters

} // namespace logika

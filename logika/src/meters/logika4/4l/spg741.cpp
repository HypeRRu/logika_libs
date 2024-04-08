/// @file Реализация класса прибора SPG741
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/spg741.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <algorithm>
/// @endcond

namespace logika
{

namespace meters
{

const std::vector< LocString > Spg741::sensors{
    LOCALIZED( "ПД1" ), LOCALIZED( "ПД2" ), LOCALIZED( "ПД3" ),
    LOCALIZED( "ПД4" ), LOCALIZED( "ПД5" ), LOCALIZED( "ТС1" ),
    LOCALIZED( "ТС2" ), LOCALIZED( "СГ1" ), LOCALIZED( "СГ2" )
};
const std::vector< std::vector< LocString > > Spg741::spParamMap{
    {
        LOCALIZED( "P1"  ), LOCALIZED( "dP3" ), LOCALIZED( "dP1" ),
        LOCALIZED( "Pб"  ), LOCALIZED( "P3"  ), LOCALIZED( "t1"  ),
        LOCALIZED( "t2"  ), LOCALIZED( "Qр1" ), LOCALIZED( "Qр2" )
    },  ///< СП=0
    {
        LOCALIZED( "P1"  ), LOCALIZED( "dP3" ), LOCALIZED( "P2"  ),
        LOCALIZED( "Pб"  ), LOCALIZED( "P3"  ), LOCALIZED( "t1"  ),
        LOCALIZED( "t2"  ), LOCALIZED( "Qр1" ), LOCALIZED( "Qр2" )
    },  ///< СП=1
    {
        LOCALIZED( "P1"  ), LOCALIZED( "dP3" ), LOCALIZED( "P2"  ),
        LOCALIZED( "dP1" ), LOCALIZED( "dP2" ), LOCALIZED( "t1"  ),
        LOCALIZED( "t2"  ), LOCALIZED( "Qр1" ), LOCALIZED( "Qр2" )
    },  ///< СП=2
    {
        LOCALIZED( "P1"  ), LOCALIZED( "dP2" ), LOCALIZED( "P2"  ),
        LOCALIZED( "Pб"  ), LOCALIZED( "dP1" ), LOCALIZED( "t1"  ),
        LOCALIZED( "t2"  ), LOCALIZED( "Qр1" ), LOCALIZED( "Qр2" )
    },  ///< СП=3
    {
        LOCALIZED( "P1"  ), LOCALIZED( "dP2" ), LOCALIZED( "P2"  ),
        LOCALIZED( "dP1" ), LOCALIZED( "P3"  ), LOCALIZED( "t1"  ),
        LOCALIZED( "t2"  ), LOCALIZED( "Qр1" ), LOCALIZED( "Qр2" )
    },  ///< СП=4
    {
        LOCALIZED( "P1"  ), LOCALIZED( "dP3" ), LOCALIZED( "dP1" ),
        LOCALIZED( "Pб"  ), LOCALIZED( "P3"  ), LOCALIZED( "t1"  ),
        LOCALIZED( "t3"  ), LOCALIZED( "Qр1" ), LOCALIZED( ""    )
    },  ///< СП=5
    {
        LOCALIZED( "P1"  ), LOCALIZED( "dP3" ), LOCALIZED( "dP1" ),
        LOCALIZED( "P3"  ), LOCALIZED( "P4"  ), LOCALIZED( "t1"  ),
        LOCALIZED( "t3"  ), LOCALIZED( "Qр1" ), LOCALIZED( ""    )
    }   ///< СП=6
};
const std::vector< LocString > Spg741::sensorVars{
    LOCALIZED( "ВД" ), LOCALIZED( "ТД" ), LOCALIZED( "ВП" ),
    LOCALIZED( "НП" ), LOCALIZED( "ЦИ" ), LOCALIZED( "КС" ),
    LOCALIZED( "КВ" ), LOCALIZED( "КН" ), LOCALIZED( "УВ" ),
    LOCALIZED( "УН" ), LOCALIZED( "Vн" )
};

Spg741::Spg741(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4L( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x4729;
    supportBaudRateChange_ = false;
    maxBaudRate_ = connections::BaudRate::Rate2400;
    sessionTimeout_ = 30 * 60; /// 30 минут
    supportFastSessionInit_ = false;

    commonTagDefs_ = {
        {
            ImportantTag::EngUnits,
            {
                  LOCALIZED( "ОБЩ.[P1]"  )
                , LOCALIZED( "ОБЩ.[dP1]" )
                , LOCALIZED( "ОБЩ.[P2]"  )
                , LOCALIZED( "ОБЩ.[dP2]" )
                , LOCALIZED( "ОБЩ.[dP3]" )
                , LOCALIZED( "ОБЩ.[Pб]"  )
                , LOCALIZED( "ОБЩ.[P3]"  )
                , LOCALIZED( "ОБЩ.[P4]"  )
            }
        },
        { ImportantTag::NetAddr, { LOCALIZED( "ОБЩ.NT" ) } },
        { ImportantTag::Ident,   { LOCALIZED( "ОБЩ.ИД" ) } },
        { ImportantTag::RDay,    { LOCALIZED( "ОБЩ.СР" ) } },
        { ImportantTag::RHour,   { LOCALIZED( "ОБЩ.ЧР" ) } }
    };

    nsDescriptions_ = {
          LOCALIZED( "Разряд батареи" ) // 00
        , LOCALIZED( "" ) // 01
        , LOCALIZED( "Перегрузка по цепям питания датчиков объема" ) // 02
        , LOCALIZED( "Активен входной сигнал ВС" ) // 03
        , LOCALIZED( "Рабочий расход Qр1 ниже нижнего предела" ) // 04
        , LOCALIZED( "Рабочий расход Qр2 ниже нижнего предела" ) // 05
        , LOCALIZED( "Рабочий расход Qр1 выше верхнего предела" ) // 06
        , LOCALIZED( "Рабочий расход Qр2 выше верхнего предела" ) // 07
        , LOCALIZED( "" ) // 08
        , LOCALIZED( "Входной сигнал по цепи Х12 вне диапазона" ) // 09
        , LOCALIZED( "Входной сигнал по цепи Х13 вне диапазона" ) // 10
        , LOCALIZED( "Входной сигнал по цепи Х14 вне диапазона" ) // 11
        , LOCALIZED( "Входной сигнал по цепи Х15 вне диапазона" ) // 12
        , LOCALIZED( "Входной сигнал по цепи Х16 вне диапазона" ) // 13
        , LOCALIZED( "Температура t1 вне диапазона" ) // 14
        , LOCALIZED( "Температура t2 вне диапазона" ) // 15
        , LOCALIZED( "Давление Р1 за пределами уставок" ) // 16
        , LOCALIZED( "Перепад давления dР1 за пределами уставок" ) // 17
        , LOCALIZED( "Рабочий расход Qр1 за пределами уставок" ) // 18
        , LOCALIZED( "Давление Р2 за пределами уставок" ) // 19
        , LOCALIZED( "Перепад давления dР2 за пределами уставок" ) // 20
        , LOCALIZED( "Рабочий расход Qр2 за пределами уставок" ) // 21
        , LOCALIZED( "Перепад давления dР3 за пределами уставок" ) // 22
        , LOCALIZED( "Давление Р3 за пределами уставок" ) // 23
        , LOCALIZED( "Давление Р4 за пределами уставок" ) // 24
        , LOCALIZED( "Объем выше нормы поставки" ) // 25
        , LOCALIZED( "Некорректные вычисления по первому трубопроводу" ) // 26
        , LOCALIZED( "Некорректные вычисления по второму трубопроводу" ) // 26
    };
} // Spg741


MeterAddressType Spg741::GetMappedDbParamAddress( const LocString& paramName, uint8_t sp )
{
    constexpr MeterAddressType dbFlashStart = 0x200;
    constexpr MeterAddressType paramSize    = 16;

    int32_t paramOrd = GetMappedDbParamOrdinal( paramName, sp );
    if ( -1 == paramOrd )
    {
        paramOrd = 103;
    }
    return dbFlashStart + paramOrd * paramSize;
} // GetMappedDbParamAddress


int32_t Spg741::GetMappedDbParamOrdinal( const LocString& paramName, uint8_t sp )
{
    constexpr MeterAddressType mappedParamStartNo   = 100;
    constexpr MeterAddressType paramsPerSensor      = 11;

    constexpr LocChar separator = LOCALIZED( '/' );
    const LocString::size_type sepPos = paramName.find_first_of( separator );

    if (   LocString::npos == sepPos 
        || LocString::npos != paramName.find_first_of( separator, sepPos + 1 ) )
    {
        throw std::runtime_error{ "Invalid param name" };
    }
    const LocString paramVar    = paramName.substr( 0, sepPos );
    const LocString paramSensor = paramName.substr( sepPos + 1 );
    if ( spParamMap.size() <= sp )
    {
        throw std::runtime_error{ "Invalid SP number: " + std::to_string( sp ) };
    }
    const std::vector< LocString >& paramMap = spParamMap.at( sp );

    const auto sensorIter   = std::find( paramMap.cbegin(), paramMap.cend(), paramSensor  );
    const auto varIter      = std::find( sensorVars.cbegin(), sensorVars.cend(), paramVar );

    if ( paramMap.cend() == sensorIter || sensorVars.cend() == varIter )
    {
        return -1;
    }
    return mappedParamStartNo
        + ( sensorIter - paramMap.cbegin() ) * paramsPerSensor
        + ( varIter - sensorVars.cbegin() );
} // GetMappedDbParamOrdinal


std::unordered_map< LocString, LocString > Spg741::BuildEuDict(
    const std::vector< std::shared_ptr< DataTag > >& euTags ) const
{
    std::unordered_map< LocString, LocString > euDict;
    /// "ОБЩ.[Р1]", "ОБЩ.[dР1]", "ОБЩ.[Р2]", "ОБЩ.[dР2]", "ОБЩ.[dР3]", "ОБЩ.[Pб]", "ОБЩ.[P3]", "ОБЩ.[P4]"
    if ( euTags.size() != 8 )
    {
        throw std::runtime_error{ "Incorrect EU tags" };
    }
    /// @note у 741 единицы измерения хранятся в двоичной(!) части параметров БД, в 2х младших битах
    for ( const auto& euTag: euTags )
    {
        if ( !euTag )
        {
            continue;
        }
        try
        {
            const LocString tagVal = euTag->GetValue< LocString >();
            uint64_t euInt = std::stoull( tagVal );
            /// @note битовая маска обязательна, тк в старших битах встречается что угодно (!)
            euDict[ euTag->GetName() ] = GetGasPressureUnits( static_cast< uint8_t >( euInt & 0x03 ) );
        }
        catch ( const std::exception& e )
        {
            euDict[ euTag->GetName() ] = LOCALIZED( "" );
        }
    }
    return euDict;
} // BuildEuDict


LocString Spg741::GetModelFromImage( const ByteVector& flashImage ) const
{
    return LOCALIZED( "" );
} // GetModelFromImage


std::vector< Logika4L::AdsFlashRun > Spg741::GetAdsFileLayout( bool all, const LocString& model ) const
{
    (void) model;
    if ( all )
    {
        return {
            Logika4L::AdsFlashRun{ 0x00000, 0x17C80 }
        };
    }
    else
    {
        return {
              Logika4L::AdsFlashRun{ 0x00000, 0x04840 }
            , Logika4L::AdsFlashRun{ 0x13440, 0x04840 }
        };
    }
} // GetAdsFileLayout

} // namespace meters

} // namespace logika

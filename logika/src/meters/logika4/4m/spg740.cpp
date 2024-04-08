/// @file Реализация класса прибора SPG740
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/spg740.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <algorithm>
/// @endcond

namespace logika
{

namespace meters
{

Spg740::Spg740(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4M( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x4728;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate57600;
    sessionTimeout_ = 1 * 60; /// 1 минута
    supportArchivePartitions_ = true;
    supportFlz_ = false;

    commonTagDefs_ = {
        {
            ImportantTag::EngUnits,
            {
                  LOCALIZED( "ОБЩ.[Pб]"  )
                , LOCALIZED( "ОБЩ.[P1]"  )
                , LOCALIZED( "ОБЩ.[dP1]" )
                , LOCALIZED( "ОБЩ.[P2]"  )
            }
        },
        { ImportantTag::SerialNo,   { LOCALIZED( "ОБЩ.serial" ) } },
        { ImportantTag::NetAddr,    { LOCALIZED( "ОБЩ.NT" ) } },
        { ImportantTag::Ident,      { LOCALIZED( "ОБЩ.ИД" ) } },
        { ImportantTag::RDay,       { LOCALIZED( "ОБЩ.СР" ) } },
        { ImportantTag::RHour,      { LOCALIZED( "ОБЩ.ЧР" ) } },
        { ImportantTag::ParamsCSum, { LOCALIZED( "ОБЩ.КСБД" ) } }
    };

    nsDescriptions_ = {
          LOCALIZED( "Разряд батареи (Uб < 3,2 В)" ) // 00
        , LOCALIZED( "Изменение сигнала на дискретном входе" ) // 01
        , LOCALIZED( "Ненулевой рабочий расход Qр1 ниже Qотс1" ) // 02
        , LOCALIZED( "Ненулевой рабочий расход Qр2 ниже Qотс2" ) // 03
        , LOCALIZED( "Рабочий расход Qр1 ниже нижнего предела, но выше Qотс1" ) // 04
        , LOCALIZED( "Рабочий расход Qр2 ниже нижнего предела, но выше Qотс2" ) // 05
        , LOCALIZED( "Рабочий расход Qр1 выше верхнего предела" ) // 06
        , LOCALIZED( "Рабочий расход Qр2 выше верхнего предела" ) // 07
        , LOCALIZED( "Измеренное значение давления датчика P1 вышло за пределы измерений датчика" ) // 08
        , LOCALIZED( "Измеренное значение давления датчика P2 вышло за пределы измерений датчика" ) // 09
        , LOCALIZED( "Измеренное значение перепада давления ΔP1 вне пределов диапазона измерений датчика" ) // 10
        , LOCALIZED( "Сигнал \"длительное состояние замкнуто\" входа V1" ) // 11
        , LOCALIZED( "Сигнал \"длительное состояние замкнуто\" входа V2" ) // 12
        , LOCALIZED( "" ) // 13
        , LOCALIZED( "" ) // 14
        , LOCALIZED( "Измеренное значение бар. давления Pб вне пределов диапазона измерений датчика" ) // 15
        , LOCALIZED( "Измеренное значение температуры t1 вне пределов диапазона -52..107 °С" ) // 16
        , LOCALIZED( "Измеренное значение температуры t2 вне пределов диапазона -52..107 °С" ) // 17
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ1 вне диапазона УН1..УВ1" ) // 18
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ2 вне диапазона УН2..УВ2" ) // 19
        , LOCALIZED( "" ) // 20
        , LOCALIZED( "Частота входного сигнала на входе V1 превышает 150 Гц" ) // 21
        , LOCALIZED( "Частота входного сигнала на входе V2 превышает 150 Гц" ) // 22
        , LOCALIZED( "Отсутствие напряжения на разъеме X1 корректора" ) // 23
        , LOCALIZED( "" ) // 24
        , LOCALIZED( "Объем выше нормы поставки" ) // 25
        , LOCALIZED( "Некорректные вычисления по первому трубопроводу" ) // 26
        , LOCALIZED( "Некорректные вычисления по второму трубопроводу" ) // 27
    };
} // Spg740


std::unordered_map< LocString, LocString > Spg740::BuildEuDict(
    const std::vector< std::shared_ptr< DataTag > >& euTags ) const
{
    std::unordered_map< LocString, LocString > euDict;
    
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
            euDict[ euTag->GetName() ] = GetGasPressureUnits( static_cast< uint8_t >( euInt ) );
        }
        catch ( const std::exception& e )
        {
            euDict[ euTag->GetName() ] = LOCALIZED( "" );
        }
    }
    return euDict;
} // BuildEuDict


std::vector< AdsTagBlock > Spg740::GetAdsTagBlocks() const
{
    return {
          AdsTagBlock{ 0, 0, 0, 55 }    ///< БД ОБЩ
        , AdsTagBlock{ 1, 1, 0, 25 }    ///< БД канал 1
        , AdsTagBlock{ 2, 2, 0, 25 }    ///< БД канал 2
        , AdsTagBlock{
            3,
            {
                LOCALIZED( "8224" ), LOCALIZED( "1024" ), LOCALIZED( "1025" ),  ///< info T D
                LOCALIZED( "1032" ), LOCALIZED( "1033" ), LOCALIZED( "1034" ),  ///< vch vpch tich
                LOCALIZED( "0.2048" ), LOCALIZED( "0.2049" ), LOCALIZED( "0.2050" ),                        ///< v vp ti
                LOCALIZED( "1.1029" ), LOCALIZED( "1.1030" ), LOCALIZED( "1.2048" ), LOCALIZED( "1.2049" ), ///< vr1ch v1ch vr1 v1
                LOCALIZED( "2.1029" ), LOCALIZED( "2.1030" ), LOCALIZED( "2.2048" ), LOCALIZED( "2.2049" )  ///< vr2ch v2ch vr2 v2                          ///< тотальные ch2
            }
        }
    };
} // GetAdsTagBlocks

} // namespace meters

} // namespace logika

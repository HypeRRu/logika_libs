/// @file Реализация класса прибора SPG742
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/spg742.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

namespace logika
{

namespace meters
{

Spg742::Spg742(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4M( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x472A;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate57600;
    sessionTimeout_ = 1 * 60; /// 1 минута
    supportFastSessionInit_ = true;
    supportArchivePartitions_ = true;
    supportFlz_ = false;

    commonTagDefs_ = {
        {
            ImportantTag::EngUnits,
            {
                  LOCALIZED( "ОБЩ.[P1]"  )
                , LOCALIZED( "ОБЩ.[dP1]" )
                , LOCALIZED( "ОБЩ.[P2]"  )
                , LOCALIZED( "ОБЩ.[dP2]" )
                , LOCALIZED( "ОБЩ.[P3]"  )
                , LOCALIZED( "ОБЩ.[dP3]" )
                , LOCALIZED( "ОБЩ.[dP4]" )
                , LOCALIZED( "ОБЩ.[Pб]"  )
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
          LOCALIZED( "Разряд батареи" ) // 00
        , LOCALIZED( "Частота входного сигнала на разъеме Х7 превышает 1,5 кГц" ) // 01
        , LOCALIZED( "Частота входного сигнала на разъеме Х8 превышает 1,5 кГц" ) // 02
        , LOCALIZED( "Изменение сигнала на дискретном входе" ) // 03
        , LOCALIZED( "Рабочий расход Qр1 ниже нижнего предела" ) // 04
        , LOCALIZED( "Рабочий расход Qр2 ниже нижнего предела" ) // 05
        , LOCALIZED( "Рабочий расход Qр1 выше верхнего предела" ) // 06
        , LOCALIZED( "Рабочий расход Qр2 выше верхнего предела" ) // 07
        , LOCALIZED( "Давление P1 вне диапазона" ) // 08
        , LOCALIZED( "Давление P2 вне диапазона") // 09
        , LOCALIZED( "Перепад давления dР1 вне диапазона" ) // 10
        , LOCALIZED( "Перепад давления dР2 вне диапазона" ) // 11
        , LOCALIZED( "Давление P3 вне диапазона" ) // 12
        , LOCALIZED( "Перепад давления dР3 вне диапазона" ) // 13
        , LOCALIZED( "Перепад давления dР4 вне диапазона" ) // 14
        , LOCALIZED( "Давление Pб вне диапазона" ) // 15
        , LOCALIZED( "Температура t1 вне диапазона" ) // 16
        , LOCALIZED( "Температура t2 вне диапазона" ) // 17
        , LOCALIZED( "Значение параметра по КУ1 вне диапазона" ) // 18
        , LOCALIZED( "Значение параметра по КУ2 вне диапазона" ) // 19
        , LOCALIZED( "Значение параметра по КУ3 вне диапазона" ) // 20
        , LOCALIZED( "Значение параметра по КУ4 вне диапазона" ) // 21
        , LOCALIZED( "Значение параметра по КУ5 вне диапазона" ) // 22
        , LOCALIZED( "Значение параметра по КУ2 вне диапазона" ) // 23
        , LOCALIZED( "" ) // 24
        , LOCALIZED( "Объем выше нормы поставки" ) // 25
        , LOCALIZED( "Некорректные вычисления по первому трубопроводу" ) // 26
        , LOCALIZED( "Некорректные вычисления по второму трубопроводу" ) // 27
        , LOCALIZED( "Измеренное значение перепада давления dP1 превышает вычисленное предельное значение, при этом Qр1>НП/Qр1" ) // 28
        , LOCALIZED( "Измеренное значение перепада давления dP2 превышает вычисленное предельное значение, при этом Qр2>НП/Qр2" ) // 27
    };
} // Spg742


std::unordered_map< LocString, LocString > Spg742::BuildEuDict(
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


std::vector< AdsTagBlock > Spg742::GetAdsTagBlocks() const
{
    return {
          AdsTagBlock{ 0, 0, 0, 64 }    ///< БД ch0
        , AdsTagBlock{ 1, 1, 0, 64 }    ///< БД ch1
        , AdsTagBlock{ 2, 2, 0, 64 }    ///< БД ch2
        , AdsTagBlock{
            3,
            {   ///< Текущие
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

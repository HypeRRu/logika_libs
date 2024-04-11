/// @file Реализация класса прибора SPT940
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/spt940.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <array>
/// @endcond

namespace logika
{

namespace meters
{

Spt940::Spt940(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4M( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x9228;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate57600;
    sessionTimeout_ = 1 * 60 * 1000; /// 1 минута
    supportArchivePartitions_ = true;
    supportFlz_ = false;

    commonTagDefs_ = {
        {
            ImportantTag::EngUnits,
            {
                  LOCALIZED( "ОБЩ.ЕИ/P" )
                , LOCALIZED( "ОБЩ.ЕИ/Q" )
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
        , LOCALIZED( "Отсутствие напряжения на разъеме X1 тепловычислителя" ) // 01
        , LOCALIZED( "Разность t между подающим и обратным труб-ми < 3 °C" ) // 02
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ1 вне диапазона УН1..УВ1" ) // 03
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ2 вне диапазона УН2..УВ2" ) // 04
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ3 вне диапазона УН3..УВ3" ) // 05
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ4 вне диапазона УН4..УВ4" ) // 06
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ5 вне диапазона УН5..УВ5" ) // 07
        , LOCALIZED( "Параметр P1 вне диапазона 0..1,03*ВП1" ) // 08
        , LOCALIZED( "Параметр P2 вне диапазона 0..1,03*ВП2" ) // 09
        , LOCALIZED( "Параметр t1 вне диапазона 0..176 °C" ) // 10
        , LOCALIZED( "Параметр t2 вне диапазона 0..176 °C" ) // 11
        , LOCALIZED( "Расход через ВС1 выше верхнего предела диапазона измерений (G1>Gв1)" ) // 12
        , LOCALIZED( "Ненулевой расход через ВС1 ниже нижнего предела диапазона измерений (0<G1<Gн1)" ) // 13
        , LOCALIZED( "Расход через ВС2 выше верхнего предела диапазона измерений (G2>Gв2)" ) // 14
        , LOCALIZED( "Ненулевой расход через ВС2 ниже нижнего предела диапазона (0<G2<Gн2)" ) // 15
        , LOCALIZED( "Расход через ВС3 выше верхнего предела диапазона измерений (G3>Gв3)" ) // 16
        , LOCALIZED( "Ненулевой расход через ВС3 ниже нижнего предела диапазона (0<G3<Gн3)" ) // 17
        , LOCALIZED( "Диагностика отрицательного значения разности часовых масс теплоносителя (М1ч–М2ч), выходящего за допустимые пределы" ) // 18
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) находится в пределах (-НМ)*М1ч <(М1ч–М2ч)<0" ) // 19
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) находится в пределах 0<(М1ч–М2ч)< НМ*М1ч" ) // 20
        , LOCALIZED( "Некорректное задание температурного графика" ) // 21
        , LOCALIZED( "Текущее значение температуры по обратному трубопроводу выше чем значение температуры, вычисленное по заданному температурному графику" ) // 22
        , LOCALIZED( "Сигнал \"длительное состояние замкнуто\" входа ВС1" ) // 23
        , LOCALIZED( "Сигнал \"длительное состояние замкнуто\" входа ВС2" ) // 24
        , LOCALIZED( "Сигнал \"длительное состояние замкнуто\" входа ВС3" ) // 25
    };
} // Spt940


std::unordered_map< LocString, LocString > Spt940::BuildEuDict(
    const std::vector< std::shared_ptr< DataTag > >& euTags ) const
{
    std::unordered_map< LocString, LocString > euDict;

    static const std::array< LocString, 3 > pUa{ LOCALIZED( "кгс/см²" ), LOCALIZED( "МПа" ), LOCALIZED( "бар" ) };
    static const std::array< LocString, 3 > qUa{ LOCALIZED( "Гкал" ), LOCALIZED( "ГДж" ), LOCALIZED( "МВт·ч" ) };

    if ( euTags.size() != 2 || !euTags.at( 0 ) || !euTags.at( 1 ) )
    {
        throw std::runtime_error{ "Incorrect EU tags" };
    }

    try
    {
        const LocString pVal = euTags.at( 0 )->GetValue< LocString >();
        uint64_t pIdx = std::stoull( pVal );
        if ( pIdx >= pUa.size() )
        {
            pIdx = 0;
        }
        euDict[ euTags.at( 0 )->GetName() ] = pUa.at( pIdx );

        const LocString qVal = euTags.at( 1 )->GetValue< LocString >();
        uint64_t qIdx = std::stoull( qVal );
        if ( qIdx >= qUa.size() )
        {
            qIdx = 0;
        }
        euDict[ euTags.at( 1 )->GetName() ] = qUa.at( qIdx );
    }
    catch ( const std::exception& )
    {
        throw std::runtime_error{ "Incorrect EU tags" };
    }
    return euDict;
} // BuildEuDict


std::vector< AdsTagBlock > Spt940::GetAdsTagBlocks() const
{
    return {
          AdsTagBlock{ 0, 0, 0, 200 }   ///< БД ch0
        , AdsTagBlock{
            3,
            {
                LOCALIZED( "8224" ), LOCALIZED( "1024" ), LOCALIZED( "1025" )   ///< info T D
            }
        }
        , AdsTagBlock{ 3, 0, 2048, 32 } ///< Тотальные (19 окр. до 32)
    };
} // GetAdsTagBlocks

} // namespace meters

} // namespace logika

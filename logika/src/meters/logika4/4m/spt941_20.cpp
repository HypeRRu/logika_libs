/// @file Реализация класса прибора SPT941_20
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/spt941_20.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <array>
/// @endcond

namespace logika
{

namespace meters
{

Spt941_20::Spt941_20(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4M( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x9229;
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
        , LOCALIZED( "Изменение сигнала на дискретном входе X4" ) // 02
        , LOCALIZED( "Изменение сигнала на дискретном входе X11" ) // 03
        , LOCALIZED( "Параметр tх вне диапазона 0..176 'C" ) // 04
        , LOCALIZED( "Параметр t4 вне диапазона -50..176 'C" ) // 05
        , LOCALIZED( "Параметр Pх вне диапазона 0..1,03*ВП3" ) // 06
        , LOCALIZED( "Параметр P4 вне диапазона 0..1,03*ВП3" ) // 07
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ1 вне диапазона УН1..УВ1" ) // 08
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ2 вне диапазона УН2..УВ2" ) // 09
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ3 вне диапазона УН3..УВ3" ) // 10
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ4 вне диапазона УН4..УВ4" ) // 11
        , LOCALIZED( "Значение контролируемого параметра, определяемого КУ5 вне диапазона УН5..УВ5" ) // 12
        , LOCALIZED( "" ) // 13
        , LOCALIZED( "" ) // 14
        , LOCALIZED( "" ) // 15
        , LOCALIZED( "" ) // 16
        , LOCALIZED( "" ) // 17
        , LOCALIZED( "" ) // 18
        , LOCALIZED( "" ) // 19
        , LOCALIZED( "" ) // 20
        , LOCALIZED( "" ) // 21
        , LOCALIZED( "" ) // 22
        , LOCALIZED( "" ) // 23
        , LOCALIZED( "" ) // 24
        , LOCALIZED( "" ) // 25
        , LOCALIZED( "" ) // 26
        , LOCALIZED( "" ) // 27
        , LOCALIZED( "" ) // 28
        , LOCALIZED( "" ) // 29
        , LOCALIZED( "" ) // 30
        , LOCALIZED( "" ) // 31
        , LOCALIZED( "Параметр P1 вне диапазона 0..1,03*ВП1" ) // 32
        , LOCALIZED( "Параметр P2 вне диапазона 0..1,03*ВП2" ) // 33
        , LOCALIZED( "Параметр P3 вне диапазона 0..1,03*ВП3" ) // 34
        , LOCALIZED( "Параметр t1 вне диапазона 0..176 'C" ) // 35
        , LOCALIZED( "Параметр t2 вне диапазона 0..176 'C" ) // 36
        , LOCALIZED( "Параметр t3 вне диапазона 0..176 'C" ) // 37
        , LOCALIZED( "Расход через ВС1 выше верхнего предела диапазона измерений (G1>Gв1)" ) // 38
        , LOCALIZED( "Ненулевой расход через ВС1 ниже нижнего предела диапазона измерений (0<G1<Gн1)" ) // 39
        , LOCALIZED( "Ненулевой расход через ВС1 ниже значения отсечки самохода (0<G1<Gотс1)" ) // 40
        , LOCALIZED( "Расход через ВС2 выше верхнего предела диапазона измерений (G2>Gв2)" ) // 41
        , LOCALIZED( "Ненулевой расход через ВС2 ниже нижнего предела диапазона (0<G2<Gн2)" ) // 42
        , LOCALIZED( "Ненулевой расход через ВС2 ниже значения отсечки самохода (0<G2<Gотс2)" ) // 43
        , LOCALIZED( "Расход через ВС3 выше верхнего предела диапазона измерений (G3>Gв3)" ) // 44
        , LOCALIZED( "Ненулевой расход через ВС3 ниже нижнего предела диапазона (0<G3<Gн3)" ) // 45
        , LOCALIZED( "Ненулевой расход через ВС3 ниже значения отсечки самохода (0<G3<Gотс3)" ) // 46
        , LOCALIZED( "Диагностика отрицательного значения разности часовых масс теплоносителя (М1ч–М2ч), выходящего за допустимые пределы" ) // 47
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) находится в пределах (-НМ)*М1ч <(М1ч–М2ч)<0" ) // 48
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) находится в пределах 0<(М1ч–М2ч)< НМ*М1ч" ) // 49
        , LOCALIZED( "Отрицательное значение часового количества тепловой энергии (Qч<0)" ) // 50
        , LOCALIZED( "Некорректное задание температурного графика" ) // 51
        , LOCALIZED( "" ) // 52
        , LOCALIZED( "Текущее значение температуры по обратному трубопроводу выше чем значение температуры, вычисленное по заданному температурному графику" ) // 53
    };
} // Spt941_20


bool Spt941_20::IdentMatch( ByteType id0, ByteType id1, ByteType version ) const
{
    return Logika4M::IdentMatch( id0, id1, version ) && version >= 0x80;
} // IdentMatch


std::unordered_map< LocString, LocString > Spt941_20::BuildEuDict(
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


std::vector< AdsTagBlock > Spt941_20::GetAdsTagBlocks() const
{
    return {
          AdsTagBlock{ 0, 0, 0, 200 }   ///< БД (167 окр. до 200)
        , AdsTagBlock{
            3,
            {
                LOCALIZED( "8224" ), LOCALIZED( "1024" ), LOCALIZED( "1025" )   ///< info T D
            }
        }
        , AdsTagBlock{ 3, 0, 2048, 32 } ///< Тотальные (27 окр. до 32)
    };
} // GetAdsTagBlocks

} // namespace meters

} // namespace logika

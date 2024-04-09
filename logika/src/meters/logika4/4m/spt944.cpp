/// @file Реализация класса прибора SPT944
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/spt944.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <array>
/// @endcond

namespace logika
{

namespace meters
{

Spt944::Spt944(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4M( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x542C;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate57600;
    sessionTimeout_ = 1 * 60; /// 1 минута
    supportArchivePartitions_ = true;
    supportFlz_ = true;

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
        , LOCALIZED( "Перегрузка по цепям питания датчиков расхода" ) // 02
        , LOCALIZED( "Изменение сигнала на порту D1 (разъем X4)" ) // 03
        , LOCALIZED( "Изменение сигнала на порту D2 (разъем X6)" ) // 04
        , LOCALIZED( "Изменение сигнала на порту D3 (разъем X5)" ) // 05
        , LOCALIZED( "Изменение сигнала на порту D4 (разъем X7)" ) // 06
        , LOCALIZED( "Датчик ТС1 вне диапазона 0..176 °C или -50..176°C" ) // 07
        , LOCALIZED( "Датчик ТС2 вне диапазона 0..176 °C или -50..176°C" ) // 08
        , LOCALIZED( "Датчик ТС3 вне диапазона 0..176 °C или -50..176°C" ) // 09
        , LOCALIZED( "Датчик ТС4 вне диапазона 0..176 °C или -50..176°C" ) // 10
        , LOCALIZED( "Датчик ТС5 вне диапазона 0..176 °C или -50..176°C" ) // 11
        , LOCALIZED( "Датчик ТС6 вне диапазона 0..176 °C или -50..176°C" ) // 12
        , LOCALIZED( "Датчик ПД1 вне диапазона 0..1,03*ВП1" ) // 13
        , LOCALIZED( "Датчик ПД2 вне диапазона 0..1,03*ВП1" ) // 14
        , LOCALIZED( "Датчик ПД3 вне диапазона 0..1,03*ВП1" ) // 15
        , LOCALIZED( "Датчик ПД4 вне диапазона 0..1,03*ВП1" ) // 16
        , LOCALIZED( "Датчик ПД5 вне диапазона 0..1,03*ВП1" ) // 17
        , LOCALIZED( "Датчик ПД6 вне диапазона 0..1,03*ВП1" ) // 18
        , LOCALIZED( "Расход через ВС1 выше верхнего предела Gв1" ) // 19
        , LOCALIZED( "Расход через ВС1 ниже нижнего предела Gн1" ) // 20
        , LOCALIZED( "Расход через ВС1 ниже отсечки самохода Gотс1" ) // 21
        , LOCALIZED( "Расход через ВС2 выше верхнего предела Gв2" ) // 22
        , LOCALIZED( "Расход через ВС2 ниже нижнего предела Gн2" ) // 23
        , LOCALIZED( "Расход через ВС2 ниже отсечки самохода Gотс2" ) // 24
        , LOCALIZED( "Расход через ВС3 выше верхнего предела Gв3" ) // 25
        , LOCALIZED( "Расход через ВС3 ниже нижнего предела Gн3" ) // 26
        , LOCALIZED( "Расход через ВС3 ниже отсечки самохода Gотс3" ) // 27
        , LOCALIZED( "Расход через ВС4 выше верхнего предела Gв4" ) // 28
        , LOCALIZED( "Расход через ВС4 ниже нижнего предела Gн4" ) // 29
        , LOCALIZED( "Расход через ВС4 ниже отсечки самохода Gотс4" ) // 30
        , LOCALIZED( "Расход через ВС5 выше верхнего предела Gв5" ) // 31
        , LOCALIZED( "Расход через ВС5 ниже нижнего предела Gн5" ) // 32
        , LOCALIZED( "Расход через ВС5 ниже отсечки самохода Gотс5" ) // 33
        , LOCALIZED( "Расход через ВС6 выше верхнего предела Gв6" ) // 34
        , LOCALIZED( "Расход через ВС6 ниже нижнего предела Gн6" ) // 35
        , LOCALIZED( "Расход через ВС6 ниже отсечки самохода Gотс6" ) // 36
        , LOCALIZED( "Значение параметра, определяемого КУ1 вне диапазона УН1..УВ1" ) // 37
        , LOCALIZED( "Значение параметра, определяемого КУ2 вне диапазона УН2..УВ2" ) // 38
        , LOCALIZED( "Значение параметра, определяемого КУ3 вне диапазона УН3..УВ3" ) // 39
        , LOCALIZED( "Значение параметра, определяемого КУ4 вне диапазона УН4..УВ4" ) // 40
        , LOCALIZED( "Значение параметра, определяемого КУ5 вне диапазона УН5..УВ5" ) // 41
        , LOCALIZED( "Ошибка описания температурного графика" ) // 42
        , LOCALIZED( "Ошибка связи с сервером" ) // 43
        , LOCALIZED( "Используется альтернативная схема учета, назначенная параметром СА1" ) // 44
        , LOCALIZED( "Используется альтернативная схема учета, назначенная параметром СА2" ) // 45
        , LOCALIZED( "" ) // 46
        , LOCALIZED( "" ) // 47
        , LOCALIZED( "" ) // 48
        , LOCALIZED( "" ) // 49
        , LOCALIZED( "" ) // 50
        , LOCALIZED( "" ) // 51
        , LOCALIZED( "" ) // 52
        , LOCALIZED( "" ) // 53
        , LOCALIZED( "Событие по расписанию 1" ) // 54
        , LOCALIZED( "Событие по расписанию 2" ) // 55
        , LOCALIZED( "" ) // 56
        , LOCALIZED( "" ) // 57
        , LOCALIZED( "" ) // 58
        , LOCALIZED( "" ) // 59
        , LOCALIZED( "" ) // 60
        , LOCALIZED( "" ) // 61
        , LOCALIZED( "" ) // 62
        , LOCALIZED( "" ) // 63
        , LOCALIZED( "ТВ1: Отрицательное значение разности часовых масс теплоносителя (М1ч–М2ч) вне допустимых пределов" ) // 64
        , LOCALIZED( "ТВ1: Значение разности часовых масс (М1ч–М2ч) находится в пределах (-НМ)*М1ч < (М1ч–М2ч) < 0" ) // 65
        , LOCALIZED( "ТВ1: Значение разности часовых масс (М1ч–М2ч) находится в пределах 0 < (М1ч–М2ч) < НМ*М1ч" ) // 66
        , LOCALIZED( "ТВ1: Отрицательное значение часового количества тепловой энергии (Qч<0)" ) // 67
        , LOCALIZED( "ТВ1: Разность температур ниже допустимого предела (dt<Уdt)" ) // 68
        , LOCALIZED( "ТВ1: Температура теплоносителя в обратном трубопроводе выше рассчитанной по температурному графику" ) // 69
        , LOCALIZED( "" ) // 70
        , LOCALIZED( "" ) // 71
        , LOCALIZED( "" ) // 72
        , LOCALIZED( "" ) // 73
        , LOCALIZED( "" ) // 74
        , LOCALIZED( "" ) // 75
        , LOCALIZED( "" ) // 76
        , LOCALIZED( "" ) // 77
        , LOCALIZED( "" ) // 78
        , LOCALIZED( "" ) // 79
        , LOCALIZED( "ТВ2: Отрицательное значение разности часовых масс теплоносителя (М1ч–М2ч) вне допустимых пределов" ) // 80
        , LOCALIZED( "ТВ2: Значение разности часовых масс (М1ч–М2ч) находится в пределах (-НМ)*М1ч < (М1ч–М2ч) < 0" ) // 81
        , LOCALIZED( "ТВ2: Значение разности часовых масс (М1ч–М2ч) находится в пределах 0 < (М1ч–М2ч) < НМ*М1ч" ) // 82
        , LOCALIZED( "ТВ2: Отрицательное значение часового количества тепловой энергии (Qч<0)" ) // 83
        , LOCALIZED( "ТВ2: Разность температур ниже допустимого предела (dt<Уdt)" ) // 84
        , LOCALIZED( "ТВ2: Температура теплоносителя в обратном трубопроводе выше рассчитанной по температурному графику" ) // 85
    };
} // Spt944


std::unordered_map< LocString, LocString > Spt944::BuildEuDict(
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


std::vector< AdsTagBlock > Spt944::GetAdsTagBlocks() const
{
    return {
          AdsTagBlock{ 0,   0, 0, 167 + 5 } ///< БД ch0   5-> запас на добавление параметров в новых версиях прибора
        , AdsTagBlock{ 100, 1, 0, 69  + 5 } ///< БД ch1
        , AdsTagBlock{ 200, 2, 0, 69  + 5 } ///< БД ch2
        , AdsTagBlock{
            3,
            {
                LOCALIZED( "8224" ), LOCALIZED( "1024" ), LOCALIZED( "1025" )   ///< info T D
            }
        }
        , AdsTagBlock{ 3,   0, 2048, 24 }   ///< Тот ОБЩ
        , AdsTagBlock{ 103, 1, 2048, 8  }   ///< Тот ТВ1
        , AdsTagBlock{ 203, 2, 2048, 8  }   ///< Тот ТВ2
    };
} // GetAdsTagBlocks

} // namespace meters

} // namespace logika

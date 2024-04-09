/// @file Реализация класса прибора SPT943rev3
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/spt943rev3.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <array>
/// @endcond

namespace logika
{

namespace meters
{

Spt943rev3::Spt943rev3(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4M( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x542B;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate19200;
    sessionTimeout_ = 1 * 60; /// 1 минута
    supportArchivePartitions_ = false;
    supportFlz_ = false;

    commonTagDefs_ = {
        { ImportantTag::SerialNo,   { LOCALIZED( "ОБЩ.serial" ) } },
        { ImportantTag::Ident,      { LOCALIZED( "ОБЩ.ИД" ) } },
        { ImportantTag::NetAddr,    { LOCALIZED( "ОБЩ.NT" ) } },
        { ImportantTag::EngUnits,   { LOCALIZED( "ОБЩ.ЕИ" ) } },
        { ImportantTag::RDay,       { LOCALIZED( "ОБЩ.СР" ) } },
        { ImportantTag::RHour,      { LOCALIZED( "ОБЩ.ЧР" ) } },
        { ImportantTag::ParamsCSum, { LOCALIZED( "ОБЩ.КСБД" ) } }
    };

    nsDescriptions_ = {
          LOCALIZED( "Разряд батареи" ) // 00
        , LOCALIZED( "Перегрузка по цепям питания преобразователей расхода" ) // 01
        , LOCALIZED( "Изменение сигнала на дискретном входе" ) // 02
        , LOCALIZED( "Параметр tхв вне диапазона 0..176°C" ) // 03
        , LOCALIZED( "Выход контролируемого параметра за границы диапазона УН..УВ" ) // 04
        , LOCALIZED( "Выход контролируемого параметра за границы диапазона УН2..УВ2" ) // 05
        , LOCALIZED( "" ) // 06
        , LOCALIZED( "Отсутствует внешнее питание" ) // 07
        , LOCALIZED( "Параметр P1 по вводу вне диапазона 0..1,1*ВП1" ) // 08
        , LOCALIZED( "Параметр P2 по вводу вне диапазона 0..1,1*ВП2" ) // 09
        , LOCALIZED( "Параметр t1 по вводу вне диапазона 0..176°C" ) // 10
        , LOCALIZED( "Параметр t2 по вводу вне диапазона 0..176°C" ) // 11
        , LOCALIZED( "Параметр t3 по вводу вне диапазона 0..176°C" ) // 12
        , LOCALIZED( "Расход через ВС1 выше верхнего предела измерений" ) // 13
        , LOCALIZED( "Ненулевой расход через ВС1 ниже нижнего предела измерений" ) // 14
        , LOCALIZED( "Расход через ВС2 выше верхнего предела измерений" ) // 15
        , LOCALIZED( "Ненулевой расход через ВС2 ниже нижнего предела измерений" ) // 16
        , LOCALIZED( "Расход через ВС3 выше верхнего предела измерений" ) // 17
        , LOCALIZED( "Ненулевой расход через ВС3 ниже нижнего предела измерений" ) // 18
        , LOCALIZED( "Отрицательное значение разности часовых масс теплоносителя(М1ч–М2ч), выходит за допустимые пределы" ) // 19
        , LOCALIZED( "Отрицательное значение часового количества тепловой энергии (Qч<0)" ) // 20
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) меньше нуля" ) // 21
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) в пределах допустимого расхождения" ) // 22
        , LOCALIZED( "Значение разности температур (dt) ниже минимального нормированного значения" ) // 23
    };
} // Spt943rev3


bool Spt943rev3::IdentMatch( ByteType id0, ByteType id1, ByteType version ) const
{
    return Logika4M::IdentMatch( id0, id1, version ) && ( version >= 0x0A && version <= 0x1F );
} // IdentMatch


std::unordered_map< LocString, LocString > Spt943rev3::BuildEuDict(
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


std::vector< AdsTagBlock > Spt943rev3::GetAdsTagBlocks() const
{
    return {
          AdsTagBlock{ 0, 0, 0, 64 }    ///< БД ch0
        , AdsTagBlock{ 100, 1, 0, 64 }  ///< БД ch1
        , AdsTagBlock{ 100, 2, 0, 64 }  ///< БД ch2
        , AdsTagBlock{
            3,
            {
                  LOCALIZED( "8224" ), LOCALIZED( "1024" )
                , LOCALIZED( "1025" ), LOCALIZED( "0.2048" )   ///< info T D Qобщ
            }
        }
        , AdsTagBlock{ 103, 1, 2048, 16 } ///< Тот ТВ1
        , AdsTagBlock{ 203, 2, 2048, 16 } ///< Тот ТВ2
    };
} // GetAdsTagBlocks

} // namespace meters

} // namespace logika

/// @file Реализация класса прибора SPT943
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/spt943.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <algorithm>
/// @endcond

namespace logika
{

namespace meters
{

Spt943::Spt943(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4L( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x542B;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate19200;
    sessionTimeout_ = 2 * 60; /// 2 минуты
    supportFastSessionInit_ = true;

    commonTagDefs_ = {
        { ImportantTag::Model,      { LOCALIZED( "ОБЩ.model" ) } },
        { ImportantTag::EngUnits,   { LOCALIZED( "ОБЩ.ЕИ"    ) } },
        { ImportantTag::NetAddr,    { LOCALIZED( "ОБЩ.NT"    ) } },
        { ImportantTag::Ident,      { LOCALIZED( "ОБЩ.ИД"    ) } },
        { ImportantTag::RDay,       { LOCALIZED( "ОБЩ.СР"    ) } },
        { ImportantTag::RHour,      { LOCALIZED( "ОБЩ.ЧР"    ) } }
    };

    nsDescriptions_ = {
          LOCALIZED( "Разряд батареи" ) // 00
        , LOCALIZED( "Перегрузка по цепям питания датчиков объема" ) // 01
        , LOCALIZED( "Изменение сигнала на дискретном входе" ) // 02
        , LOCALIZED( "Параметр tхв вне диапазона" ) // 03
        , LOCALIZED( "Выход контролируемого параметра за границы диапазона" ) // 04
        , LOCALIZED( "" ) // 05
        , LOCALIZED( "" ) // 06
        , LOCALIZED( "" ) // 07
        , LOCALIZED( "Параметр P1 вне диапазона" ) // 08
        , LOCALIZED( "Параметр P2 вне диапазона" ) // 09
        , LOCALIZED( "Параметр t1 вне диапазона" ) // 10
        , LOCALIZED( "Параметр t2 вне диапазона" ) // 11
        , LOCALIZED( "Параметр t3 вне диапазона" ) // 12
        , LOCALIZED( "Расход через ВС1 выше верхнего предела" ) // 13
        , LOCALIZED( "Ненулевой расход через ВС1 ниже нижнего предела") // 14
        , LOCALIZED( "Расход через ВС2 выше верхнего предела" ) // 15
        , LOCALIZED( "Ненулевой расход через ВС2 ниже нижнего предела" ) // 16
        , LOCALIZED( "Расход через ВС3 выше верхнего предела" ) // 17
        , LOCALIZED( "Ненулевой расход через ВС3 ниже нижнего предела" ) // 18
        , LOCALIZED( "Отрицательное значение разности часовых масс теплоносителя, выходящее за допустимые пределы" ) // 19
        , LOCALIZED( "Отрицательное значение часового количества тепловой энергии" ) // 20
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) меньше нуля" ) // 21
    };

    CalcFieldDefSettings cfDefSettings1;
    cfDefSettings1.channelNo        = 1;
    cfDefSettings1.ordinal          = -1;
    cfDefSettings1.name             = LOCALIZED( "dt" );
    cfDefSettings1.stdVar           = StdVar::T;
    cfDefSettings1.description      = LOCALIZED( "dt ТВ1" );
    cfDefSettings1.type             = DbType::Single;
    cfDefSettings1.dbType           = LOCALIZED( "" );
    cfDefSettings1.displayFormat    = LOCALIZED( "0.00" );
    cfDefSettings1.insertAfter      = LOCALIZED( "ТВ1_t2" );
    cfDefSettings1.expression       = LOCALIZED( "ТВ1_t1-ТВ1_t2" );
    cfDefSettings1.eu               = LOCALIZED( "°C" );

    CalcFieldDefSettings cfDefSettings2{ cfDefSettings1 };
    cfDefSettings2.channelNo        = 2;
    cfDefSettings1.description      = LOCALIZED( "dt ТВ2" );
    cfDefSettings1.insertAfter      = LOCALIZED( "ТВ2_t2" );
    cfDefSettings1.expression       = LOCALIZED( "ТВ2_t1-ТВ2_t2" );

    std::shared_ptr< ChannelDef > cTv = nullptr;
    auto cDefIter = std::find_if( channels_.begin(), channels_.end(), []( const auto& def ){
        return def && def->kind == ChannelKind::TV && def->start == 1;
    } );

    if ( channels_.end() != cDefIter )
    {
        cTv = *cDefIter;
        calcFields_ = {
              std::make_shared< CalcFieldDef >( *cTv, cfDefSettings1 )
            , std::make_shared< CalcFieldDef >( *cTv, cfDefSettings2 )
        };
    }
} // Spt943


bool Spt943::IdentMatch( ByteType id0, ByteType id1, ByteType version ) const
{
    return Logika4L::IdentMatch( id0, id1, version ) && version <= 0x0A;
} // IdentMatch


std::unordered_map< LocString, LocString > Spt943::BuildEuDict(
    const std::vector< std::shared_ptr< DataTag > >& euTags ) const
{
    std::unordered_map< LocString, LocString > euDict;

    if ( euTags.size() != 1 || !euTags.at( 0 )
        || euTags.at( 0 )->GetName() == LOCALIZED( "ЕИ" ) )
    {
        throw std::runtime_error{ "Incorrect EU tags" };
    }

    try
    {
        const LocString sVal = euTags.at( 0 )->GetValue< LocString >();
        uint64_t sEu = std::stoull( sVal );
        
        switch ( sEu )
        {
            case 1:
                euDict[ LOCALIZED( "[P]" ) ] = LOCALIZED( "МПа" );
                euDict[ LOCALIZED( "[Q]" ) ] = LOCALIZED( "ГДж" );
                break;
            case 2:
                euDict[ LOCALIZED( "[P]" ) ] = LOCALIZED( "бар" );
                euDict[ LOCALIZED( "[Q]" ) ] = LOCALIZED( "МВт·ч" );
                break;
            /// В прибор можно ввести значение и больше 2,
            /// прибор будет использовать в этом случае значение по умолчанию - 0
            case 0:
            default:
                euDict[ LOCALIZED( "[P]" ) ] = LOCALIZED( "кгс/см²" );
                euDict[ LOCALIZED( "[Q]" ) ] = LOCALIZED( "Гкал" );
                break;
        }
    }
    catch ( const std::exception& )
    {
        throw std::runtime_error{ "Incorrect EU tags" };
    }
    return euDict;
} // BuildEuDict


LocString Spt943::GetModelFromImage( const ByteVector& flashImage ) const
{
    constexpr MeterAddressType modelAddress = 0x30;
    if ( flashImage.size() <= modelAddress )
    {
        return LOCALIZED( "" );
    }
    return LocString( static_cast< LocChar >( flashImage.at( 0x30 ) ), 1 );
} // GetModelFromImage


std::vector< Logika4L::AdsFlashRun > Spt943::GetAdsFileLayout( bool all, const LocString& model ) const
{
    (void) model;
    if ( all )
    {
        return {
            Logika4L::AdsFlashRun{ 0x00000, 0x3A980 }
        };
    }
    else
    {
        return {
              Logika4L::AdsFlashRun{ 0x00000, 0x08CC0 }
            , Logika4L::AdsFlashRun{ 0x1AEC0, 0x06E00 }
            , Logika4L::AdsFlashRun{ 0x33B80, 0x06E00 }
        };
    }
} // GetAdsFileLayout

} // namespace meters

} // namespace logika

/// @file Реализация класса прибора SPT941_10
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/spt941_10.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

namespace logika
{

namespace meters
{

Spt941_10::Spt941_10(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4L( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x9229;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate19200;
    sessionTimeout_ = 2 * 60 * 1000; /// 2 минуты
    supportFastSessionInit_ = true;

    commonTagDefs_ = {
        { ImportantTag::Model,      { LOCALIZED( "ОБЩ.model" ) } },
        { ImportantTag::EngUnits,   { LOCALIZED( "ОБЩ.ЕИ"    ) } },
        { ImportantTag::RDay,       { LOCALIZED( "ОБЩ.СР"    ) } },
        { ImportantTag::RHour,      { LOCALIZED( "ОБЩ.ЧР"    ) } },
        { ImportantTag::NetAddr,    { LOCALIZED( "ОБЩ.NT"    ) } },
        { ImportantTag::Ident,      { LOCALIZED( "ОБЩ.ИД"    ) } }
    };

    nsDescriptions_ = {
          LOCALIZED( "Разряд батареи" ) // 00
        , LOCALIZED( "Перегрузка по цепям питания датчиков объема" ) // 01
        , LOCALIZED( "Изменение сигнала на дискретном входе" ) // 02
        , LOCALIZED( "" ) // 03
        , LOCALIZED( "Выход контролируемого параметра за границы диапазона" ) // 04
        , LOCALIZED( "" ) // 05
        , LOCALIZED( "" ) // 06
        , LOCALIZED( "" ) // 07
        , LOCALIZED( "Параметр t1 вне диапазона" ) // 08
        , LOCALIZED( "Параметр t2 вне диапазона" ) // 09
        , LOCALIZED( "Расход через ВС1 выше верхнего предела" ) // 10
        , LOCALIZED( "Ненулевой расход через ВС1 ниже нижнего предела" ) // 11
        , LOCALIZED( "Расход через ВС2 выше верхнего предела" ) // 12
        , LOCALIZED( "Ненулевой расход через ВС2 ниже нижнего предела" ) // 13
        , LOCALIZED( "Расход через ВС3 выше верхнего предела" ) // 14
        , LOCALIZED( "Ненулевой расход через ВС3 ниже нижнего предела" ) // 15
        , LOCALIZED( "Отрицательное значение разности часовых масс теплоносителя, выходящее за допустимые пределы" ) // 16
        , LOCALIZED( "Отрицательное значение часового количества тепловой энергии" ) // 17
        , LOCALIZED( "Значение разности часовых масс (М1ч–М2ч) меньше нуля" ) // 18
    };


    CalcFieldDefSettings cfDefSettings;
    cfDefSettings.channelNo     = 0;
    cfDefSettings.ordinal       = -1;
    cfDefSettings.name          = LOCALIZED( "dt" );
    cfDefSettings.stdVar        = StdVar::T;
    cfDefSettings.description   = LOCALIZED( "dt" );
    cfDefSettings.type          = DbType::Single;
    cfDefSettings.dbType        = LOCALIZED( "" );
    cfDefSettings.displayFormat = LOCALIZED( "0.00" );
    cfDefSettings.insertAfter   = LOCALIZED( "t2" );
    cfDefSettings.expression    = LOCALIZED( "t1-t2" );
    cfDefSettings.eu            = LOCALIZED( "°C" );

    if ( !channels_.empty() && channels_.at( 0 ) )
    {
        calcFields_ = {
            std::make_shared< CalcFieldDef >( *channels_.at( 0 ), cfDefSettings )
        };
    }
} // Spt941_10


bool Spt941_10::IdentMatch( ByteType id0, ByteType id1, ByteType version ) const
{
    return Logika4L::IdentMatch( id0, id1, version ) && static_cast< uint8_t >( version ) < 0x80;
} // IdentMatch


std::unordered_map< LocString, LocString > Spt941_10::BuildEuDict(
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


LocString Spt941_10::GetModelFromImage( const ByteVector& flashImage ) const
{
    constexpr MeterAddressType modelAddress = 0x30;
    if ( flashImage.size() <= modelAddress )
    {
        return LOCALIZED( "" );
    }
    return LOCALIZED( "1" ) + LocString( static_cast< LocChar >( flashImage.at( 0x30 ) ), 1 );
} // GetModelFromImage


std::vector< Logika4L::AdsFlashRun > Spt941_10::GetAdsFileLayout( bool all, const LocString& model ) const
{
    (void) model;
    if ( all )
    {
        return {
              Logika4L::AdsFlashRun{ 0x00000, 0x01200 }
            , Logika4L::AdsFlashRun{ 0x04000, 0x12880 }
        };
    }
    else
    {
        return {
              Logika4L::AdsFlashRun{ 0x00000, 0x01200 }
            , Logika4L::AdsFlashRun{ 0x04000, 0x027C0 }
            , Logika4L::AdsFlashRun{ 0x12140, 0x04740 }
        };
    }
} // GetAdsFileLayout

} // namespace meters

} // namespace logika

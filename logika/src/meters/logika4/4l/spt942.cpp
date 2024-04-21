/// @file Реализация класса прибора SPT942
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/spt942.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>
#include <logika/meters/logika4/4l/archive_def4l.h>

/// @cond
#include <algorithm>
/// @endcond

namespace // anonymous
{

logika::meters::ArchiveDef4LSettings CopySettingsForTv2(
    const std::shared_ptr< logika::meters::ArchiveDef4L >& arcDef )
{
    logika::meters::ArchiveDef4LSettings adSettings;
    adSettings.archType         = arcDef->GetArchiveType();
    adSettings.type             = arcDef->GetElementType();
    adSettings.capacity         = arcDef->GetCapacity();
    adSettings.name             = arcDef->GetName();
    adSettings.description      = arcDef->GetDescription();
    adSettings.recordSize       = arcDef->GetRecordSize();
    adSettings.indexAddr        = 0xFFFFFFFF; // -1
    adSettings.headerAddr       = 0xFFFFFFFF; // -1
    adSettings.recordsAddr      = 0xFFFFFFFF; // -1
    adSettings.indexAddr2       = arcDef->GetIndexAddr2();
    adSettings.headerAddr2      = arcDef->GetHeaderAddr2();
    adSettings.recordsAddr2     = arcDef->GetRecordsAddr();
    adSettings.isTiny42         = true;
    return adSettings;
} // CopySettingsForTv2

} // anonymous namespace

namespace logika
{

namespace meters
{

Spt942::Spt942(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4L( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x542A;
    supportBaudRateChange_ = true;
    maxBaudRate_ = connections::BaudRate::Rate9600;
    sessionTimeout_ = 30 * 60 * 1000; /// 30 минут
    supportFastSessionInit_ = false;

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
        , LOCALIZED( "Перегрузка по цепям питания датчиков объема и давления" ) // 01
        , LOCALIZED( "" ) // 02
        , LOCALIZED( "" ) // 03
        , LOCALIZED( "" ) // 04
        , LOCALIZED( "" ) // 05
        , LOCALIZED( "" ) // 06
        , LOCALIZED( "" ) // 07
        , LOCALIZED( "Параметр P1 вне диапазона" ) // 08
        , LOCALIZED( "Параметр P2 вне диапазона" ) // 09
        , LOCALIZED( "Параметр t1 вне диапазона" ) // 10
        , LOCALIZED( "Параметр t2 вне диапазона" ) // 11
        , LOCALIZED( "Расход через ВС1 выше верхнего предела" ) // 12
        , LOCALIZED( "Ненулевой расход через ВС1 ниже нижнего предела") // 13
        , LOCALIZED( "Расход через ВС2 выше верхнего предела" ) // 14
        , LOCALIZED( "Ненулевой расход через ВС2 ниже нижнего предела" ) // 15
        , LOCALIZED( "Расход через ВС3 выше верхнего предела" ) // 16
        , LOCALIZED( "Ненулевой расход через ВС3 ниже нижнего предела" ) // 17
        , LOCALIZED( "Абсолютное значение отрицательной часовой массы М3ч больше, чем 4 % часовой массы М1ч" ) // 18
        , LOCALIZED( "Отрицательное значение часового количества тепловой энергии" ) // 19
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
    auto cDefIter = std::find_if( channels_.begin(), channels_.end(), []( const std::shared_ptr< ChannelDef >& def ) {
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
} // Spt942


void Spt942::Init( const storage::StorageKeeper& sKeeper )
{
    Logika4L::Init( sKeeper );

    auto arcTypesStorage    = sKeeper.GetStorage< LocString, ArchiveType >();
    auto devicesStorage     = sKeeper.GetStorage< LocString, Meter >();
    if ( !arcTypesStorage )
    {
        throw std::runtime_error{ "Archive types storage not present" };
    }
    if ( !devicesStorage )
    {
        throw std::runtime_error{ "Devices storage not present" };
    }
    
    std::shared_ptr< ArchiveType > arcTypeHour = nullptr;
    arcTypesStorage->GetItem( LOCALIZED( "Hour" ), arcTypeHour );
    std::shared_ptr< ArchiveType > arcTypeDay = nullptr;
    arcTypesStorage->GetItem( LOCALIZED( "Day" ), arcTypeDay );
    std::shared_ptr< ArchiveType > arcTypeMonth = nullptr;
    arcTypesStorage->GetItem( LOCALIZED( "Month" ), arcTypeMonth );

    if ( !arcTypeHour || !arcTypeDay || !arcTypeMonth )
    {
        throw std::runtime_error{ "Archive type Hour, Day or Month not found" };
    }

    auto hourIter = std::find_if( archives_.begin(), archives_.end(), [ arcTypeHour ](
        const std::shared_ptr< ArchiveDef >& arc ) {
        return arc && arc->GetArchiveType() == arcTypeHour;
    } );
    auto dayIter = std::find_if( archives_.begin(), archives_.end(), [ arcTypeDay ](
        const std::shared_ptr< ArchiveDef >& arc ) {
        return arc && arc->GetArchiveType() == arcTypeDay;
    } );
    auto monthIter = std::find_if( archives_.begin(), archives_.end(), [ arcTypeMonth ](
        const std::shared_ptr< ArchiveDef >& arc ) {
        return arc && arc->GetArchiveType() == arcTypeMonth;
    } );

    if (   archives_.end() == hourIter || archives_.end() == dayIter || archives_.end() == monthIter )
    {
        throw std::runtime_error{ "Archive Hour, Day or Month not found" };
    }

    std::shared_ptr< ArchiveDef4L > hourArc     = std::dynamic_pointer_cast< ArchiveDef4L >( *hourIter );
    std::shared_ptr< ArchiveDef4L > dayArc      = std::dynamic_pointer_cast< ArchiveDef4L >( *dayIter );
    std::shared_ptr< ArchiveDef4L > monthArc    = std::dynamic_pointer_cast< ArchiveDef4L >( *monthIter );

    if ( !hourArc || !dayArc || !monthArc )
    {
        throw std::runtime_error{ "Archive Hour, Day or Month is invalid" };
    }

    auto tv2ChanIter = std::find_if( channels_.begin(), channels_.end(), [](
        const std::shared_ptr< ChannelDef >& channel ) {
        return channel && channel->start == 1 && channel->count == 1;
    } );
    std::shared_ptr< ChannelDef > m46tv2 = nullptr;
    if ( channels_.end() == tv2ChanIter )
    {
        std::shared_ptr< Meter > currentDev = nullptr;
        devicesStorage->GetItem( GetCaption(), currentDev );
        /// @todo warning if not found
        m46tv2 = std::make_shared< ChannelDef >( currentDev, hourArc->GetChannelDef().prefix
            , 2, 1, LOCALIZED( "канал ТВ2 в одноканальных СПТ942 (мод. 4/6)" ) );
        channels_.push_back( m46tv2 );
    }
    else
    {
        m46tv2 = *tv2ChanIter;
    }

    if ( m46tv2 )
    {
        /// Архивы для моделей 4, 6 (одноканальные, ТВ1 отсутствует, по ТВ2 индекс и заголовки на своём месте,
        /// данные находятся на месте данных ТВ1)
        archives_.emplace_back( std::make_shared< ArchiveDef4L >(
            *m46tv2, CopySettingsForTv2( hourArc ) ) );
        archives_.emplace_back( std::make_shared< ArchiveDef4L >(
            *m46tv2, CopySettingsForTv2( dayArc ) ) );
        archives_.emplace_back( std::make_shared< ArchiveDef4L >(
            *m46tv2, CopySettingsForTv2( monthArc ) ) );
    }
} // Init


std::unordered_map< LocString, LocString > Spt942::BuildEuDict(
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


LocString Spt942::GetModelFromImage( const ByteVector& flashImage ) const
{
    constexpr MeterAddressType modelAddress = 0x30;
    if ( flashImage.size() <= modelAddress )
    {
        return LOCALIZED( "" );
    }
    return LocString( static_cast< LocChar >( flashImage.at( 0x30 ) ), 1 );
} // GetModelFromImage


std::vector< Logika4L::AdsFlashRun > Spt942::GetAdsFileLayout( bool all, const LocString& model ) const
{
    bool bothTvs = false;
    if (   model == LOCALIZED( "1" ) || model == LOCALIZED( "2" )
        || model == LOCALIZED( "3" ) || model == LOCALIZED( "5" ) )
    {
        bothTvs = true;
    }
    else if ( model == LOCALIZED( "4" ) || model == LOCALIZED( "6" ) )
    {
        bothTvs = false;
    }
    else
    {
        throw std::runtime_error{ "Unsupported SPT942 model" };
    }

    
    if ( all )
    {
        if ( bothTvs )
        {
            return { Logika4L::AdsFlashRun{ 0x00000, 0x2A800 } };
        }
        else
        {
            return { Logika4L::AdsFlashRun{ 0x00000, 0x19D00 } };
        }
    }
    else
    {
        std::vector< AdsFlashRun > flash{
              Logika4L::AdsFlashRun{ 0x00000, 0x07300 }
            , Logika4L::AdsFlashRun{ 0x151C0, 0x04B40 }
        };
        if ( bothTvs )
        {
            // У моделей 4 и 6 нет архива по второму вводу
            flash.emplace_back(
                Logika4L::AdsFlashRun{ 0x27840, 0x02FC0 }
            );
        }
        return flash;
    }
} // GetAdsFileLayout

} // namespace meters

} // namespace logika

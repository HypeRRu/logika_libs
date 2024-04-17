/// @file Реализация протокола M4 (API)
/// @copyright HypeRRu 2024
/// @see m4protocol_impl.cpp

#include <logika/protocols/m4/m4protocol.h>

#include <logika/log/defines.h>
#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/meters/logika4/4l/spg741.h>
#include <logika/meters/logika4/4l/tag_def4l.h>
#include <logika/meters/data_tag.h>
#include <logika/common/misc.h>

/// @cond
#include <algorithm>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{


M4Protocol::MeterCache::MeterCache( M4Protocol* owner, std::shared_ptr< meters::Logika4 > meter, ByteType nt )
    : bus_{ owner }
    , meter_{ meter }
    , nt_{ nt }
    , model_{}
    , modelSet_{ false }
    , sp_{ static_cast< ByteType >( -1 ) }
    , rd_{ static_cast< MeterAddressType >( -1 ) }
    , rh_{ static_cast< MeterAddressType >( -1 ) }
    , euDict_{}
    , deviceClockDiff_{ static_cast< TimeType >( -1 ) }
    , clockDiffSet_{ false }
    , flash_{}
    , flashPageMap_{}
    , vipTags_{}
{
    std::shared_ptr< meters::Logika4L > meter4L = std::dynamic_pointer_cast< meters::Logika4L >( meter_ );
    if ( meter4L )
    {
        const auto tags = meter->GetTagsVault();
        if ( !tags )
        {
            return;
        }
        const std::vector< std::shared_ptr< meters::DataTagDef > >& tagsList = tags->All();
        MeterAddressType lastTotalAddr = 0x0;
        for ( std::shared_ptr< meters::DataTagDef > tag: tagsList )
        {
            if ( !tag || tag->GetKind() != meters::TagKind::TotalCtr )
            {
                continue;
            }
            std::shared_ptr< meters::TagDef4L > tag4L = std::dynamic_pointer_cast< meters::TagDef4L >( tag );
            if ( !tag4L )
            {
                throw std::logic_error{ "Tag type need to be TagDef4L" };
            }
            MeterAddressType address = tag4L->GetAddress() + tag4L->GetChannelOffset()
                + meters::Logika4L::SizeOfType( tag4L->GetInternalType() );
            if ( address > lastTotalAddr )
            {
                lastTotalAddr = address;
            }
        }
        MeterAddressType paramsFlashSize = lastTotalAddr + meters::Logika4L::FLASH_PAGE_SIZE - 1; // запас для хвостов
        flash_          = ByteVector( paramsFlashSize, 0x0 );
        flashPageMap_   = std::vector <bool >( flash_.size() / meters::Logika4L::FLASH_PAGE_SIZE );
    }
    if ( meter_ )
    {
        vipTags_ = meter_->GetWellKnownTags();
    }
} // MeterCache


const M4Protocol* M4Protocol::MeterCache::GetBus() const
{
    return bus_;
} // GetBus const


M4Protocol* M4Protocol::MeterCache::GetBus()
{
    return bus_;
} // GetBus


const std::shared_ptr< meters::Logika4 > M4Protocol::MeterCache::GetMeter() const
{
    return meter_;
} // GetMeter const


std::shared_ptr< meters::Logika4 > M4Protocol::MeterCache::GetMeter()
{
    return meter_;
} // GetMeter


ByteType M4Protocol::MeterCache::GetNt() const
{
    return nt_;
} // GetNt


LocString M4Protocol::MeterCache::GetModel() const
{
    if ( !model_.empty() || modelSet_ )
    {
        return model_;
    }
    modelSet_ = true;
    if ( bus_ && vipTags_.count( meters::ImportantTag::Model ) )
    {
        bus_->UpdateTagsImpl( &nt_, vipTags_[ meters::ImportantTag::Model ], TagsUpdateFlags::DontGetEus );
        if ( !vipTags_[ meters::ImportantTag::Model ].empty() )
        {
            vipTags_[ meters::ImportantTag::Model ].at( 0 )->TryGetValue< LocString >( model_ );
        }
    }
    return model_;
} // GetModel


ByteType M4Protocol::MeterCache::GetSp() const
{
    return sp_;
} // GetSp


void M4Protocol::MeterCache::SetSp( ByteType sp )
{
    sp_ = sp;
} // SetSp


MeterAddressType M4Protocol::MeterCache::GetRd() const
{
    if ( rd_ != static_cast< MeterAddressType >( -1 ) )
    {
        return rd_;
    }
    LoadRdRh();
    return rd_;
} // GetRd


MeterAddressType M4Protocol::MeterCache::GetRh() const
{
    if ( rh_ != static_cast< MeterAddressType >( -1 ) )
    {
        return rh_;
    }
    LoadRdRh();
    return rh_;
} // GetRh


void M4Protocol::MeterCache::LoadRdRh() const
{
    if ( !bus_
        || vipTags_.count( meters::ImportantTag::RDay )
        || vipTags_.at( meters::ImportantTag::RDay ).empty()
        || vipTags_.count( meters::ImportantTag::RHour )
        || vipTags_.at( meters::ImportantTag::RHour ).empty() )
    {
        return;
    }
    std::vector< std::shared_ptr< meters::DataTag > > tags{
          vipTags_[ meters::ImportantTag::RDay  ][ 0 ]
        , vipTags_[ meters::ImportantTag::RHour ][ 0 ]
    };
    bus_->UpdateTagsImpl( &nt_, tags, TagsUpdateFlags::DontGetEus );

    try
    {
        LocString rdValue = tags[ 0 ]->GetValue< LocString >();
        rd_ = static_cast< MeterAddressType >( std::stoull( rdValue, nullptr, 10 ) );
    }
    catch ( const std::exception& )
    {
        LOG_WRITE( LOG_WARNING, LOCALIZED( "Could not convert RDay value" ) );
    }

    try
    {
        LocString rhValue = tags[ 1 ]->GetValue< LocString >();
        rh_ = static_cast< MeterAddressType >( std::stoull( rhValue, nullptr, 10 ) );
    }
    catch ( const std::exception& )
    {
        LOG_WRITE( LOG_WARNING, LOCALIZED( "Could not convert RHour value" ) );
    }
} // LoadRdRh


TimeType M4Protocol::MeterCache::GetCurrentDeviceTime() const
{
    if ( deviceClockDiff_ != static_cast< TimeType >( -1 ) )
    {
        return GetCurrentTimestamp() - deviceClockDiff_;
    }
    if ( clockDiffSet_ )
    {
        return 0;
    }

    clockDiffSet_ = true;
    if ( !bus_ || !meter_ || !meter_->GetTagsVault() )
    {
        return 0;
    }
    auto tagsVault = meter_->GetTagsVault();
    std::shared_ptr< meters::DataTagDef > tagTime = tagsVault->Find( LOCALIZED( "ОБЩ" ), LOCALIZED( "T" ) );
    std::shared_ptr< meters::DataTagDef > tagDate = tagsVault->Find( LOCALIZED( "ОБЩ" ), LOCALIZED( "Д" ) );
    if ( !tagTime || !tagDate )
    {
        /// Устройство без часов (например ЛГК410)
        return 0;
    }
    std::vector< std::shared_ptr< meters::DataTag > > tags{
          std::make_shared< meters::DataTag >( tagDate, 0 )
        , std::make_shared< meters::DataTag >( tagTime, 0 )
    };
    bus_->UpdateTagsImpl( &nt_, tags, TagsUpdateFlags::DontGetEus );
    LocString dateValue{};
    LocString timeValue{};
    tags.at( 0 )->TryGetValue< LocString >( dateValue );
    tags.at( 1 )->TryGetValue< LocString >( timeValue );
    
    TimeType currentTime    = GetCurrentTimestamp();
    TimeType devType        = meters::Logika4::CombineDateTime( dateValue, timeValue );
    deviceClockDiff_        = currentTime - devType;
    return GetCurrentTimestamp() - deviceClockDiff_;
} // GetCurrentDeviceTime


const std::unordered_map< LocString, LocString >& M4Protocol::MeterCache::GetEuDict() const
{
    if ( !euDict_.empty() )
    {
        return euDict_;
    }
    if ( vipTags_.count( meters::ImportantTag::EngUnits ) && bus_ && meter_ )
    {
        bus_->UpdateTagsImpl( &nt_, vipTags_[ meters::ImportantTag::EngUnits ], TagsUpdateFlags::DontGetEus );
        euDict_ = meter_->BuildEuDict( vipTags_[ meters::ImportantTag::EngUnits ] );
    }
    return euDict_;
} // GetEuDict

/// -----------------------------------

void M4Protocol::UpdateTags( const ByteType* srcNt, const ByteType* dstNt,
    std::vector< std::shared_ptr< meters::DataTag > >& tags )
{
    (void) srcNt;
    if ( tags.empty() )
    {
        return;
    }
    UpdateTagsImpl( dstNt, tags, TagsUpdateFlags::None );
} // UpdateTags


void M4Protocol::UpdateTagsImpl( const ByteType* nt, std::vector< std::shared_ptr< meters::DataTag > >& tags,
    TagsUpdateFlags flags )
{
    /// @todo Реализовать
} // UpdateTagsImpl


std::shared_ptr< M4Protocol::MeterCache > M4Protocol::GetMeterInstance(
    std::shared_ptr< meters::Logika4 > meter,const ByteType* nt )
{
    ByteType effNt = nt ? *nt : M4Protocol::BROADCAST_NT;

    if ( 0 == metadataCache_.count( effNt ) )
    {
        metadataCache_[ effNt ] = std::make_shared< M4Protocol::MeterCache >(
            this, meter, effNt
        );
    }
    return metadataCache_[ effNt ];
} // GetMeterInstance


void M4Protocol::GetFlashPagesToCache( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
    uint32_t startPage, uint32_t count, std::shared_ptr< M4Protocol::MeterCache > meterInstance )
{
    if ( !meter || !meterInstance || 0 == count )
    {
        throw std::invalid_argument{ "Invalid arguments passed" };
    }
    int64_t rangeStart = -1;
    uint32_t rangeSize =  0;

    for ( uint32_t i = 0; i < count; ++i )
    {
        uint32_t page = startPage + i;
        /// @bug rangeSize ведь отражает количество подряд идущих страниц,
        /// а разрывы в flashPageMap_ в теории могут быть не подряд
        /// @note Будет чтение именно подряд идущих сегментов
        if ( !meterInstance->flashPageMap_.at( page ) ) /// Страницы нет в кэше
        {
            if ( rangeStart < 0 )
            {
                rangeStart = page;
            }
            ++rangeSize;
            if ( i != count - 1 )
            {
                continue;
            }
        }
        else if ( rangeStart < 0 )
        {
            continue; /// Нет подряд идущих незагруженных страниц
        }
        /// Загрузка подряд идущих незагруженных страниц
        LOG_WRITE( LOG_INFO, LOCALIZED( "Load to cache flash pages " ) << rangeStart
            << LOCALIZED( ".." ) << ( rangeStart + rangeSize ) );
        ByteVector pages = ReadFlashPagesL4( meter, nt, rangeStart, rangeSize );
        constexpr MeterAddressType pageSize = meters::Logika4L::FLASH_PAGE_SIZE;
        for ( uint32_t rp = 0; rp < rangeSize; ++rp )
        {
            MeterAddressType pageOffset  = pageSize * rp;
            MeterAddressType flashOffset = ( rp + rangeStart ) * pageSize;
            for ( MeterAddressType addr = 0; addr < pageSize; ++addr )
            {
                meterInstance->flash_[ flashOffset + addr ] = pages[ pageOffset + addr ];
            }
            meterInstance->flashPageMap_[ rangeStart + rp ] = true;
        }
        rangeStart = -1;
        rangeSize  = 0;
    }
} // GetFlashPagesToCache


MeterAddressType M4Protocol::GetRealAddress4L( std::shared_ptr< M4Protocol::MeterCache > meterInstance,
    std::shared_ptr< meters::DataTag > tag )
{
    std::shared_ptr< meters::TagDef4L > def = std::dynamic_pointer_cast< meters::TagDef4L >( tag->GetDef() );
    if ( !meterInstance || !meterInstance->GetMeter() || !def )
    {
        return 0;
    }
    std::shared_ptr< meters::Logika4 > meter = meterInstance->GetMeter();
    ByteType nt = meterInstance->GetNt();
    if ( meter->GetCaption() == LOCALIZED( "СПГ741" )
        && def->GetOrdinal() >= 200 && def->GetOrdinal() <= 300 )
    {
        return meters::Spg741::GetMappedDbParamAddress( def->GetKey(), GetSpg741Sp( &nt ) );
    }
    else
    {
        MeterAddressType channelOff = def->GetChannelOffset();
        return def->GetAddress() + ( ( channelOff && tag->GetChannel().no == 2 ) ? channelOff : 0 );
    }
} // GetRealAddress4L

} // namespace M4

} // namespace protocols

} // namespace logika

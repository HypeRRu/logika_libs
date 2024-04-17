/// @file Реализация протокола M4 (API)
/// @copyright HypeRRu 2024
/// @see m4protocol_impl.cpp

#include <logika/protocols/m4/m4protocol.h>

#include <logika/log/defines.h>
#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/meters/logika4/4l/spg741.h>
#include <logika/meters/logika4/4l/tag_def4l.h>
#include <logika/meters/logika4/4m/logika4m.h>
#include <logika/meters/logika4/4m/tag_def4m.h>
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
    if ( meter_ )
    {
        vipTags_ = meter_->GetWellKnownTags();
    }
    std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >( meter_ );
    if ( !meter4l )
    {
        return;
    }
    /// Настройки, специфичные для Logika4L
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
        LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Could not convert RDay value" ) );
    }

    try
    {
        LocString rhValue = tags[ 1 ]->GetValue< LocString >();
        rh_ = static_cast< MeterAddressType >( std::stoull( rhValue, nullptr, 10 ) );
    }
    catch ( const std::exception& )
    {
        LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Could not convert RHour value" ) );
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
    TagsUpdateFlags::Type flags )
{
    if ( tags.empty() || !tags.at( 0 ) || !tags.at( 0 )->GetDef() )
    {
        return;
    }
    std::shared_ptr< meters::Logika4 > meter4 = std::dynamic_pointer_cast< meters::Logika4 >(
        tags.at( 0 )->GetDef()->GetMeter() );
    if ( !meter4 )
    {
        LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Invalid tags passed" ) );
        return;
    }
    std::shared_ptr< M4Protocol::MeterCache > mtrInstance = GetMeterInstance( meter4, nt );
    if ( !mtrInstance )
    {
        throw std::logic_error{ "Meter cache not set" };
    }
    std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >( meter4 );
    std::shared_ptr< meters::Logika4M > meter4m = std::dynamic_pointer_cast< meters::Logika4M >( meter4 );
    if ( meter4l )
    {
        UpdateTagsValues4L( nt, tags, mtrInstance, flags );
    }
    else if ( meter4m )
    {
        UpdateTags4M( nt, tags, mtrInstance, flags );
    }
    else
    {
        LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Meter is not Logika4L or Logika4M" ) );
    }
} // UpdateTagsImpl


std::shared_ptr< M4Protocol::MeterCache > M4Protocol::GetMeterInstance(
    std::shared_ptr< meters::Logika4 > meter, const ByteType* nt )
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
    if ( meter->GetCaption() == LOCALIZED( "SPG741" )
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


void M4Protocol::UpdateTagsValues4L( const ByteType* nt, const std::vector< std::shared_ptr< meters::DataTag > >& tags,
    std::shared_ptr< MeterCache > meterInstance, TagsUpdateFlags::Type flags )
{
    if ( !meterInstance || tags.empty() || !tags.at( 0 )
        || !tags.at( 0 )->GetDef() )
    {
        return;
    }
    std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >(
        tags.at( 0 )->GetDef()->GetMeter() );
    if ( !meter4l )
    {
        throw std::runtime_error{ "Invalid meter instance" };
    }
    for ( std::shared_ptr< meters::DataTag > tag: tags )
    {
        if ( !tag )
        {
            continue;
        }
        std::shared_ptr< meters::TagDef4L > def = std::dynamic_pointer_cast< meters::TagDef4L >( tag->GetDef() );
        if ( !def )
        {
            LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Tag def not set" ) );
            continue;
        }
        tag->SetEu( def->GetUnits() );
        MeterAddressType address = GetRealAddress4L( meterInstance, tag );
        uint32_t startPage = address / meters::Logika4L::FLASH_PAGE_SIZE;
        bool oper = false;
        if ( def->IsInRam() )
        {
            ByteVector buffer = ReadRamL4( meter4l, nt, address,
                meters::Logika4L::SizeOfType( def->GetInternalType() ) );
            tag->ReplaceValue( meters::Logika4L::GetValue( def->GetInternalType(), buffer, 0, oper ) );
        }
        else // flash (or flash + ram)
        {
            // some naive read-ahead, to reduce request count
            uint32_t pfCount = ( startPage < meterInstance->flashPageMap_.size() - 1 ) ? ( startPage % 2 ) : 0;
            GetFlashPagesToCache( meter4l, nt, startPage, pfCount + 1, meterInstance );
            tag->ReplaceValue( meters::Logika4L::GetValue( def->GetInternalType(),
                meterInstance->flash_, address, oper ) );
            tag->SetOper( oper );
            /// Тотальные счетчики из двух частей (i32r32 во Flash + r32 в RAM)
            if ( def->GetAddonAddress() )
            {
                MeterAddressType channelOff = def->GetAddonChannelOffset();
                MeterAddressType ramPartAddr = def->GetAddonAddress()
                    + ( ( channelOff && tag->GetChannel().no == 2 ) ? channelOff : 0 );

                ByteVector buffer = ReadRamL4( meter4l, nt, ramPartAddr,
                    meters::Logika4L::SizeOfType( meters::BinaryType4L::R32 ) );
                double value = 0.0;
                if ( !tag->TryGetValue( value ) )
                {
                    LOG_WRITE_MSG( LOG_ERROR, LOCALIZED( "Unable to cast value to I32R32 result" ) );
                }
                value += meters::Logika4L::ConvertMFloat( buffer, 0 );
                tag->SetValue( value );
            }
        }
        if ( !( flags & TagsUpdateFlags::DontGetEus ) )
        {
            tag->SetEu( meters::Logika4::GetEu( meterInstance->euDict_, def->GetUnits() ) );
        }
        tag->SetTimestamp( GetCurrentTimestamp() );
        PostProcessValue( tag );
    }
} // UpdateTagsValues4L


void M4Protocol::InvalidateFlashCache4L( const ByteType* nt,
    const std::vector< std::shared_ptr< meters::DataTag > >& tags )
{
    if ( tags.empty() || !tags.at( 0 ) || !tags.at( 0 )->GetDef() )
    {
        return;
    }
    std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >(
        tags.at( 0 )->GetDef()->GetMeter() );
    std::shared_ptr< M4Protocol::MeterCache > mtrInstance = GetMeterInstance( meter4l, nt );
    if ( !mtrInstance )
    {
        throw std::logic_error{ "Meter cache cannot be null" };
    }
    for ( auto tag: tags )
    {
        if ( !tag )
        {
            continue;
        }
        std::shared_ptr< meters::TagDef4L > def = std::dynamic_pointer_cast< meters::TagDef4L >( tag->GetDef() );
        if ( !def )
        {
            LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Tag def not set" ) );
            continue;
        }
        MeterAddressType address = GetRealAddress4L( mtrInstance, tag );
        uint32_t startPage = address / meters::Logika4L::FLASH_PAGE_SIZE;
        uint32_t endPage   = ( address + meters::Logika4L::SizeOfType( def->GetInternalType() ) - 1 )
            / meters::Logika4L::FLASH_PAGE_SIZE;
        for ( uint32_t page = startPage; page <= endPage; ++page )
        {
            mtrInstance->flashPageMap_[ page ] = false;
        }
    }
} // InvalidateFlashCache4L


void M4Protocol::PostProcessValue( std::shared_ptr< meters::DataTag > tag )
{
    if ( !tag || !tag->GetDef() )
    {
        return;
    }
    std::shared_ptr< meters::Meter > mtr = tag->GetDef()->GetMeter();
    if ( mtr->GetCaption() != LOCALIZED( "SPT941_10" )
        || !StrCaseEq( tag->GetName(), LOCALIZED( "model" ) )
        || !tag->HasValue()
    )
    {
        return;
    }
    LocString value;
    if ( !tag->TryGetValue( value )
        || value.length() != 1 )
    {
        return;
    }
    /// СПТ941.10/11 хранит код модели последней цифрой
    tag->SetValue< LocString >( LOCALIZED( "1" ) + value );
} // PostProcessValue


void M4Protocol::UpdateTags4M( const ByteType* nt, const std::vector< std::shared_ptr< meters::DataTag > >& tags,
    std::shared_ptr< MeterCache > meterInstance, TagsUpdateFlags::Type flags )
{
    if ( !meterInstance || tags.empty() || !tags.at( 0 )
        || !tags.at( 0 )->GetDef() )
    {
        return;
    }
    std::shared_ptr< meters::Logika4M > meter4m = std::dynamic_pointer_cast< meters::Logika4M >(
        tags.at( 0 )->GetDef()->GetMeter() );
    if ( !meter4m )
    {
        LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Invalid tags" ) );
        return;
    }

    std::vector< int32_t > channels{};
    std::vector< int32_t > ordinals{};

    uint32_t blockStart = 0;
    for ( size_t i = 0; i < tags.size(); ++i )
    {
        std::shared_ptr< meters::DataTag > tag = tags[ i ];
        if ( !tag )
        {
            continue;
        }
        std::shared_ptr< meters::TagDef4M > def = std::dynamic_pointer_cast< meters::TagDef4M >( tag->GetDef() );
        if ( !def )
        {
            LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Tag def not set" ) );
            continue;
        }
        channels.push_back( tag->GetChannel().no );
        ordinals.push_back( tag->GetOrdinal() );

        if ( ordinals.size() >= M4Protocol::MAX_TAGS_AT_ONCE
            || i == tags.size() - 1 )
        {
            std::vector< bool > opFlags;
            auto values = ReadTags4M( meter4m, nt, channels, ordinals, opFlags );
            for ( size_t j = 0; j < ordinals.size(); ++j )
            {
                std::shared_ptr< meters::DataTag > valTag = tags[ blockStart + j ];
                if ( !valTag )
                {
                    LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Tag not set" ) );
                    continue;
                }
                valTag->ReplaceValue( values[ j ].second );
                if ( !valTag->HasValue() )
                {
                    valTag->SetErrorDescription( meters::Logika4M::ND_STR );
                }
                if ( !( flags & TagsUpdateFlags::DontGetEus ) )
                {
                    std::shared_ptr< meters::TagDef4M > def4m = std::dynamic_pointer_cast< meters::TagDef4M >(
                        valTag->GetDef() );
                    if ( def4m )
                    {
                        valTag->SetEu( meters::Logika4::GetEu( meterInstance->euDict_, def4m->GetUnits() ) );
                    }
                }
                valTag->SetOper( opFlags[ j ] );
                valTag->SetTimestamp( GetCurrentTimestamp() );
            }

            blockStart += ordinals.size();
            channels.clear();
            ordinals.clear();
        }
    }
} // UpdateTags4M

} // namespace M4

} // namespace protocols

} // namespace logika

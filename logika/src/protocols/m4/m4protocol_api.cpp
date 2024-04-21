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
#include <logika/meters/interval_archive.h>
#include <logika/meters/service_archive.h>
#include <logika/meters/logika4/archive_field_def4.h>
#include <logika/meters/logika4/4l/archive_def4l.h>
#include <logika/meters/logika4/4m/archive_def4m.h>
#include <logika/meters/logika4/4m/archive_field_def4m.h>
#include <logika/protocols/m4/flash_archive4l.h>
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
        /// @note Чтение именно подряд идущих сегментов
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


std::shared_ptr< meters::IntervalArchive > M4Protocol::ReadIntervalArchiveDef( std::shared_ptr< meters::Meter > meter,
    const ByteType* srcNt, const ByteType* dstNt, std::shared_ptr< meters::ArchiveType > archiveType,
    std::shared_ptr< logika::Any >& state )
{
    (void) srcNt;
    if ( !archiveType || !archiveType->IsIntervalArchive() )
    {
        throw std::invalid_argument{ "Wrong archive type" };
    }
    std::shared_ptr< meters::Logika4 > meter4 = std::dynamic_pointer_cast< meters::Logika4 >( meter );
    if ( !meter )
    {
        throw std::invalid_argument{ "Wrong meter" };
    }
    std::shared_ptr< meters::IntervalArchive > archive = std::make_shared< meters::IntervalArchive >( meter, archiveType );
    std::shared_ptr< M4Protocol::MeterCache > mtrInstance = GetMeterInstance( meter4, dstNt );
    
    std::shared_ptr< meters::ArchiveDef > archiveDef = nullptr;
    const auto& archives = meter4->GetArchives();
    if ( meter4->GetCaption() == LOCALIZED( "SPT942" ) )
    {
        const LocString model = mtrInstance->GetModel();
        const bool tiny42 = ( model == LOCALIZED( "4" ) || model == LOCALIZED( "6" ) );
        auto arDefIter = std::find_if( archives.cbegin(), archives.cend(), [ tiny42, archiveType ](
            const std::shared_ptr< meters::ArchiveDef >& def ) {
            std::shared_ptr< meters::ArchiveDef4L > def4l = std::dynamic_pointer_cast< meters::ArchiveDef4L >( def );
            if ( !def4l )
            {
                return false;
            }
            return def4l->GetArchiveType() == archiveType && ( def4l->IsTiny() == tiny42 );
        } );
        if ( archives.cend() != arDefIter )
        {
            archiveDef = *arDefIter;
        }
    }
    else
    {
        auto arDefIter = std::find_if( archives.cbegin(), archives.cend(), [ archiveType ](
            const std::shared_ptr< meters::ArchiveDef >& def ) {
            return def && def->GetArchiveType() == archiveType;
        } );
        if ( archives.cend() != arDefIter )
        {
            archiveDef = *arDefIter;
        }
    }
    if ( !archiveDef )
    {
        throw std::runtime_error{ "Invalid archive def" };
    }
    std::vector< std::shared_ptr< meters::ArchiveFieldDef4 > > fieldDefs{};
    const auto& fieldDefsAll = meter4->GetArchiveFields();
    for ( const auto& fieldDef: fieldDefsAll )
    {
        std::shared_ptr< meters::ArchiveFieldDef4 > fieldDef4 = std::dynamic_pointer_cast< meters::ArchiveFieldDef4 >(
            fieldDef );
        if ( !fieldDef4 )
        {
            continue;
        }
        if ( fieldDef4->GetArchiveType() == archiveType )
        {
            fieldDefs.emplace_back( fieldDef4 );
        }
    }
    int32_t channelStart = archiveDef->GetChannelDef().start;
    int32_t channelEnd   = archiveDef->GetChannelDef().count + channelStart - 1;
    
    for ( int32_t channel = channelStart; channel <= channelEnd; ++channel )
    {
        for ( std::shared_ptr< meters::ArchiveFieldDef4 > fieldDef: fieldDefs )
        {
            std::shared_ptr< meters::ArchiveField > archiveField = std::make_shared< meters::ArchiveField >(
                fieldDef, channel );
            archiveField->SetEu( meters::Logika4::GetEu( mtrInstance->euDict_, fieldDef->GetUnits() ) );
            LocString fieldName = fieldDef->GetName();
            if ( archiveDef->GetChannelDef().kind == meters::ChannelKind::TV )
            {
                fieldName = archiveDef->GetChannelDef().prefix
                    + ToLocString( std::to_string( channel ) )
                    + LOCALIZED( "_" ) + fieldName;
            }
            archive->GetDataTable()->AddColumn( archiveField );
        }
    }
    state = nullptr;
    if ( std::dynamic_pointer_cast< meters::Logika4L >( meter4 ) )
    {
        std::vector< TvReadState4L > tvStates;
        std::shared_ptr< meters::ArchiveDef4L > archiveDef4l = std::dynamic_pointer_cast< meters::ArchiveDef4L >( archiveDef );
        if ( !archiveDef4l )
        {
            throw std::runtime_error{ "Invalid archive def" };
        }
        for ( int32_t i = 0; i < archiveDef->GetChannelDef().count; ++i )
        {
            tvStates.emplace_back();
            tvStates.back().headersRead = false;
            tvStates.back().index = -1;
            tvStates.back().fArchive = std::make_shared< SyncFlashArchive4 >( mtrInstance, archiveDef4l,
                archiveDef4l->GetChannelDef().start + i );
        }
        std::shared_ptr< ArchiveRequestState4L > requestState = std::make_shared< ArchiveRequestState4L >();
        requestState->tvReadStates = tvStates;
        state = std::make_shared< logika::Any >( requestState );
    }
    else if ( std::dynamic_pointer_cast< meters::Logika4M >( meter4 ) )
    {
        std::shared_ptr< meters::ArchiveDef4M > archiveDef4m = std::dynamic_pointer_cast< meters::ArchiveDef4M >( archiveDef );
        if ( !archiveDef4m )
        {
            throw std::runtime_error{ "Invalid archive def" };
        }
        std::vector< std::shared_ptr< meters::ArchiveFieldDef4M > > fieldDefs4m;
        for ( std::shared_ptr< meters::ArchiveFieldDef4 > fieldDef: fieldDefs )
        {
            fieldDefs4m.emplace_back( std::dynamic_pointer_cast< meters::ArchiveFieldDef4M >( fieldDef ) );
        }
        std::shared_ptr< ArchiveRequestState4M > requestState = std::make_shared< ArchiveRequestState4M >();
        requestState->archiveDef = archiveDef4m;
        requestState->fieldsDef  = fieldDefs4m;
        state = std::make_shared< logika::Any >( requestState );
    }
    else
    {
        throw std::runtime_error{ "Invalid Logika4 Meter" };
    }

    return archive;
} // ReadIntervalArchiveDef


bool M4Protocol::ReadIntervalArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
    const ByteType* dstNt, std::shared_ptr< meters::IntervalArchive > archive, TimeType start, TimeType end,
    std::shared_ptr< logika::Any >& state, double& progress )
{
    (void) srcNt;
    std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >( meter );
    std::shared_ptr< meters::Logika4M > meter4m = std::dynamic_pointer_cast< meters::Logika4M >( meter );
    if ( meter4l )
    {
        return ReadFlashArchive4L( meter4l, dstNt, archive, start, end, state, progress );
    }
    else if ( meter4m )
    {
        return ReadIntervalArchive4M( meter4m, dstNt, archive, start, end, state, progress );
    }
    else
    {
        throw std::invalid_argument{ "Wrong meter type" };
    }
    return false;
} // ReadIntervalArchive


bool M4Protocol::ReadServiceArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
    const ByteType* dstNt, std::shared_ptr< meters::ServiceArchive > archive, TimeType start, TimeType end,
    std::shared_ptr< logika::Any >& state, double& progress )
{
    (void) srcNt;
    if ( !archive || !archive->GetArchiveType() )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }
    if ( !archive->GetArchiveType()->IsServiceArchive() )
    {
        throw std::invalid_argument{ "Wrong archive type" };
    }
    std::shared_ptr< meters::Logika4L > meter4l = std::dynamic_pointer_cast< meters::Logika4L >( meter );
    std::shared_ptr< meters::Logika4M > meter4m = std::dynamic_pointer_cast< meters::Logika4M >( meter );
    if ( meter4l )
    {
        return ReadFlashArchive4L( meter4l, dstNt, archive, start, end, state, progress );
    }
    else if ( meter4m )
    {
        return ReadServiceArchive4M( meter4m, dstNt, archive, start, end, state, progress );
    }
    else
    {
        throw std::invalid_argument{ "Wrong meter type" };
    }
    return false;
} // ReadServiceArchive


TimeType M4Protocol::GetDeviceClock( std::shared_ptr< meters::Meter > meter,
    const ByteType* srcNt, const ByteType* dstNt )
{
    (void) srcNt;
    std::shared_ptr< meters::Logika4 > meter4 = std::dynamic_pointer_cast< meters::Logika4 >( meter );
    std::shared_ptr< M4Protocol::MeterCache > mtrInstance = GetMeterInstance( meter4, dstNt );
    return mtrInstance->GetCurrentDeviceTime();
} // GetDeviceClock


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


bool M4Protocol::ReadFlashArchive4L( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
    std::shared_ptr< meters::Archive > archive, TimeType start, TimeType end,
    std::shared_ptr< logika::Any >& state, double& progress )
{
    std::shared_ptr< M4Protocol::MeterCache > mtrInstance = GetMeterInstance( meter, nt );
    std::shared_ptr< ArchiveRequestState4L > requestState = nullptr;
    if ( !meter || !archive || !archive->GetArchiveType() )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }

    std::shared_ptr< meters::ArchiveType > archiveType = archive->GetArchiveType();
    if ( !state || ( !state->Empty()
        && !state->TryCast< std::shared_ptr< ArchiveRequestState4L > >( requestState ) ) )
    {
        throw std::invalid_argument{ "Invalid archive request state" };
    }
    if ( !requestState )
    {
        if ( archiveType->IsIntervalArchive() )
        {
            throw std::invalid_argument{ "Invalid interval archive request state" };
        }
        /// Чтение сервисного архива
        requestState = InitServiceArchiveReadState4L( meter, nt, archiveType );
        state = std::make_shared< logika::Any >( requestState );
    }

    /// Процентное соотношение заголовков и данных ко всем данным
    uint32_t headersPercentage  = 0;
    uint32_t dataPercentage     = 0;
    if ( archiveType->IsIntervalArchive() )
    {
        headersPercentage   = 10 / requestState->tvReadStates.size();
        dataPercentage      = 100 / requestState->tvReadStates.size() - headersPercentage;
    }
    else
    {
        headersPercentage   = 100 / requestState->tvReadStates.size();
        dataPercentage      = 0;
    }

    for ( size_t i = 0; i < requestState->tvReadStates.size(); ++i )
    {
        auto tvReadState = requestState->tvReadStates[ i ];
        std::shared_ptr< FlashArchive4 > flashArchive = tvReadState.fArchive;
        if ( tvReadState.index < 0 )
        {
            std::vector< uint32_t > newHeaders{};
            uint32_t index = 0;
            flashArchive->GetHeaders()->ManageOutdatedElements( true, newHeaders, index );
            tvReadState.index = index;
        }
        double headersCompleted = 0.0;
        if ( !tvReadState.headersRead )
        {
            if ( flashArchive->GetHeaders()->GetElementIndicesInRange( start, end, tvReadState.index,
                tvReadState.restartPoint, tvReadState.indices, headersCompleted ) )
            {
                tvReadState.headersRead = true;
                tvReadState.dirtyIndices = tvReadState.indices;
                std::sort( tvReadState.dirtyIndices.begin(), tvReadState.dirtyIndices.end(), FrbIndex::LessByIdx );
            }
            else
            {
                progress = ( i * ( headersPercentage + dataPercentage )
                    + ( headersCompleted * headersPercentage / 100.0 ) );
                return true; // headers not read completely yet
            }
        }
        flashArchive->UpdateData( tvReadState.dirtyIndices );
        if ( tvReadState.dirtyIndices.size() > 0 ) // data update not completed yet
        {
            if ( tvReadState.dirtyInitialCount > 0 )
            {
                double dataCompleted = 100.0 * ( tvReadState.dirtyInitialCount
                    - tvReadState.dirtyIndices.size() ) / tvReadState.dirtyIndices.size();
                progress = ( i * ( headersPercentage + dataPercentage )
                    + headersPercentage + ( dataCompleted * dataPercentage / 100.0 ) );
            }
            else
            {
                progress = 0.0;
            }
            return true;
        }
    }

    progress = 100.0;
    if ( archiveType->IsIntervalArchive() )
    {
        std::shared_ptr< meters::IntervalArchive > intervalArchive
            = std::dynamic_pointer_cast< meters::IntervalArchive >( archive );
        ProcessIntervalData4L( requestState, intervalArchive );
    }
    else
    {
        std::shared_ptr< meters::ServiceArchive > serviceArchive
            = std::dynamic_pointer_cast< meters::ServiceArchive >( archive );
        ProcessServiceData4L( requestState, serviceArchive );
    }
    return false; // no more data
} // ReadFlashArchive4L


bool M4Protocol::ReadIntervalArchive4M( std::shared_ptr< meters::Logika4M > meter, const ByteType* nt,
    std::shared_ptr< meters::IntervalArchive > archive, TimeType start, TimeType end,
    std::shared_ptr< logika::Any >& state, double& progress )
{
    constexpr uint32_t readRecordsCount = 64;

    std::shared_ptr< M4Protocol::MeterCache > mtrInstance = GetMeterInstance( meter, nt );
    std::shared_ptr< ArchiveRequestState4M > requestState = nullptr;
    if ( !meter || !archive )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }
    if ( !state || !state->TryCast< std::shared_ptr< ArchiveRequestState4M > >( requestState ) || !requestState )
    {
        throw std::invalid_argument{ "Invalid archive request state" };
    }
    ArchiveId4M::Type archiveCode = GetArchiveCode( archive->GetArchiveType() );

    int32_t channelStart = requestState->archiveDef->GetChannelDef().start;
    int32_t channelEnd   = requestState->archiveDef->GetChannelDef().count + channelStart - 1;
    if ( -1 == requestState->currentChannel )
    {
        requestState->currentChannel        = channelStart;
        requestState->nChannelRecordsRead   = 0;
    }

    /// @note По разным каналам прибор может вернуть разное количество записей (при одинаковом интервале в запросе)
    /// поэтому каналы читаем по очереди
    TimeType timeStart = ( 0 != requestState->timePointer ) ? requestState->timePointer : start;
    
    std::vector< std::shared_ptr< ArchiveRecord > > data{};
    TimeType nextPointer = 0;
    ReadArchive4M( meter, nt, 0, archivePartition_, static_cast< ByteType >( requestState->currentChannel ),
        archiveCode, timeStart, end, readRecordsCount, data, nextPointer );

    std::vector< meters::DataTable::RecordType >& records = archive->GetDataTable()->GetRecordsList();
    for ( auto record: data )
    {
        FixIntervalTimestamp( record, archive->GetArchiveType(), mtrInstance );
        TimeType time = record->fullTime;
        /// Под индексом 0 расположен столбец tm
        auto iter = std::find_if( records.begin(), records.end(), [ time ](
            const meters::DataTable::RecordType& record ) {
            TimeType recTime;
            return record && record->at( 0 )
                && record->at( 0 )->TryCast< TimeType >( recTime )
                && recTime == time;
        } );
        /// Добавление/обновление записи
        meters::DataTable::RecordType row = nullptr;
        if ( records.end() == iter || !( *iter ) )
        {
            const size_t recordSize = archive->GetDataTable()->FieldCount();
            meters::DataTable::RecordType record =
                std::make_shared< std::vector< std::shared_ptr< logika::Any > > >( recordSize, nullptr );
            ( *record )[ 0 ] = std::make_shared< logika::Any >( time );
            row = record;
            archive->GetDataTable()->PushRecord( record );
        }
        else
        {
            row = *iter;
        }
        /// Копирование записи в архив
        const size_t channelRecIdx = 1 + requestState->fieldsDef.size()
            * ( requestState->currentChannel - channelStart ); // Индекс начала полей для данного канала
        for ( size_t i = 0; i < record->values.size(); ++i )
        {
            ( *row )[ i + channelRecIdx ] = record->values.at( i );
        }
        ++requestState->nChannelRecordsRead;
    }
    
    TimeType dataStart = start;
    if ( records.size() > 0 )
    {
        if ( !records.front()->at( 0 ) )
        {
            throw std::runtime_error{ "Invalid record" };
        }
        dataStart = records.front()->at( 0 )->Cast< TimeType >();
    }
    uint32_t intervalsCount = ( end - dataStart ) / archive->GetArchiveType()->GetInterval();
    uint32_t intervalsPerChannel = std::min( requestState->archiveDef->GetCapacity(), intervalsCount );
    uint32_t totalRecordsParts = intervalsPerChannel * requestState->archiveDef->GetChannelDef().count;
    uint32_t numParts = ( requestState->currentChannel - channelStart )
        * intervalsPerChannel + requestState->nChannelRecordsRead;
    progress = std::min( 100.0, 100.0 * numParts / static_cast< double >( totalRecordsParts ) );

    requestState->timePointer = nextPointer;
    if ( nextPointer == 0 || nextPointer > end )
    {
        requestState->timePointer = 0;
        requestState->nChannelRecordsRead = 0;
        ++requestState->currentChannel;
    }
    bool hasMoreData = requestState->currentChannel <= channelEnd && requestState->timePointer <= end;

    return hasMoreData;
} // ReadIntervalArchive4M


bool M4Protocol::ReadServiceArchive4M( std::shared_ptr< meters::Logika4M > meter, const ByteType* nt,
    std::shared_ptr< meters::ServiceArchive > archive, TimeType start, TimeType end,
    std::shared_ptr< logika::Any >& state, double& progress )
{
    constexpr uint32_t readRecordsCount = 64;

    if ( !meter || !archive || !archive->GetArchiveType() || !state )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }

    std::shared_ptr< meters::ArchiveType > archiveType = archive->GetArchiveType();
    ArchiveId4M::Type archiveCode = ArchiveId4M::Undefined;
    if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "ParamsLog" ) ) )
    {
        archiveCode = ArchiveId4M::ParamsLog;
    }
    else if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "ErrorsLog" ) ) )
    {
        archiveCode = ArchiveId4M::NsLog;
    }
    else
    {
        throw std::runtime_error{ "Unsupported archive type" };
    }

    auto archives = meter->GetArchives();
    auto iter = std::find_if( archives.begin(), archives.end(), [ archiveType ](
        const std::shared_ptr< meters::ArchiveDef >& def ) {
        return def && def->GetArchiveType() == archiveType;
    } );
    if ( archives.end() == iter )
    {
        throw std::runtime_error{ "Archive def not found" };
    }
    std::shared_ptr< meters::ArchiveDef4M > arDef4m = std::dynamic_pointer_cast< meters::ArchiveDef4M >( *iter );
    if ( !arDef4m )
    {
        throw std::runtime_error{ "Invalid archive def" };
    }

    int32_t channelStart    = arDef4m->GetChannelDef().start;
    int32_t channelEnd      = arDef4m->GetChannelDef().count + channelStart - 1;

    TimeType timePointer = 0;
    if ( state )
    {
        if ( !state->TryCast< TimeType >( timePointer ) )
        {
            LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Invalid state value" ) );
        }
    }

    std::vector< TimeType > nextPtrs( arDef4m->GetChannelDef().count, 0 );
    TimeType startTime = 0 != timePointer ? timePointer : start;
    std::vector< meters::ServiceArchive::RecordType > recordsList{};

    for ( int32_t channel = channelStart; channel <= channelEnd; ++channel )
    {
        std::vector< std::shared_ptr< ArchiveRecord > > data{};
        TimeType nextPointer = 0;
        ReadArchive4M( meter, nt, 0, archivePartition_, static_cast< ByteType >( channel ),
            archiveCode, startTime, end, readRecordsCount, data, nextPointer );
    
        for ( std::shared_ptr< ArchiveRecord > record: data  )
        {
            recordsList.emplace_back( ArchiveRecordToService( meter, archive->GetArchiveType(), channel, record ) );
        }
        nextPtrs[ channel - channelStart ] = nextPointer;
    }

    timePointer = 0;
    for ( TimeType nextPtr: nextPtrs )
    {
        if ( 0 != nextPtr && nextPtr > timePointer )
        {
            timePointer = nextPtr;
        }
    }

    for ( meters::ServiceArchive::RecordType record: recordsList )
    {
        archive->AddRecord( record );
    }
    TimeType firstRecTime = archive->GetRecords().size() ? archive->GetRecords().front()->timestamp : start;
    *state = timePointer;

    if ( 0 == timePointer )
    {
        progress = 100.0;
    }
    else
    {
        progress = 100.0 * ( timePointer - firstRecTime ) / static_cast< double >( end - firstRecTime );
    }

    return 0 != timePointer && timePointer < end;
} // ReadServiceArchive4M


std::shared_ptr< ArchiveRequestState4L > M4Protocol::InitServiceArchiveReadState4L(
    std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
    std::shared_ptr< meters::ArchiveType > archiveType )
{
    std::shared_ptr< M4Protocol::MeterCache > mtrInstance = GetMeterInstance( meter, nt );
    std::shared_ptr< meters::ArchiveDef > arDef = nullptr;
    auto archives = meter->GetArchives();
    auto iter = std::find_if( archives.begin(), archives.end(), [ archiveType ](
        const std::shared_ptr< meters::ArchiveDef >& def ) {
        return def && def->GetArchiveType() == archiveType;
    } );
    if ( archives.end() != iter )
    {
        throw std::runtime_error{ "No archive def found" };
    }
    arDef = *iter;
    std::shared_ptr< ArchiveRequestState4L > requestState = std::make_shared< ArchiveRequestState4L >();

    std::shared_ptr< FlashRingBuffer::GetObjectF< LocString > > recordGetter = nullptr;
    if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "ErrorsLog" ) ) )
    {
        recordGetter = std::make_shared< FlashRingBuffer::GetObjectF< LocString > >(
            []( FlashArchive4* archive, const ByteVector& buffer, MeterAddressType offset ) {
                (void) archive;
                bool oper = false;
                return meters::Logika4L::GetNsRecordValue( buffer, offset, oper );
            }
        );
    }
    else if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "ParamsLog" ) ) )
    {
        recordGetter = std::make_shared< FlashRingBuffer::GetObjectF< LocString > >(
            []( FlashArchive4* archive, const ByteVector& buffer, MeterAddressType offset ) {
                (void) archive;
                bool oper = false;
                return meters::Logika4L::GetIzmRecordValue( buffer, offset, oper );
            }
        );
    }

    std::shared_ptr< meters::ArchiveDef4L > arDef4l = std::dynamic_pointer_cast< meters::ArchiveDef4L >( arDef );
    if ( !arDef4l )
    {
        throw std::runtime_error{ "Invalid archive def" };
    }
    for ( size_t i = 0; i < arDef->GetChannelDef().count; ++i )
    {
        requestState->tvReadStates.emplace_back();
        requestState->tvReadStates.back().fArchive = std::make_shared< AsyncFlashArchive4 >(
            mtrInstance, arDef4l,
            static_cast< int32_t >( arDef4l->GetChannelDef().start + i ),
            recordGetter
        );
        requestState->tvReadStates.back().headersRead = false;
        requestState->tvReadStates.back().index = -1;
    }

    return requestState;
} // InitServiceArchiveReadState4L


void M4Protocol::ProcessIntervalData4L( std::shared_ptr< ArchiveRequestState4L > state,
    std::shared_ptr< meters::IntervalArchive > archive )
{
    if ( !state || !archive )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }
    archive->GetDataTable()->Clear();
    
    std::vector< meters::DataTable::RecordType >& records = archive->GetDataTable()->GetRecordsList();
    for ( size_t tv = 0; tv < state->tvReadStates.size(); ++tv )
    {
        TvReadState4L& trs = state->tvReadStates[ tv ];
        for ( size_t i = 0; i < trs.indices.size(); ++i )
        {
            if ( trs.fArchive )
            {
                continue;
            }
            std::shared_ptr< meters::VQT > dataPoint = trs.fArchive->GetDataPoint( trs.indices.at( i ).index );
            if ( !dataPoint || !dataPoint->value )
            {
                continue;
            }
            meters::DataTable::RecordType row = nullptr;
            TimeType time = dataPoint->timestamp;
            /// Под индексом 0 расположен столбец tm
            auto iter = std::find_if( records.begin(), records.end(), [ time ](
                const meters::DataTable::RecordType& record ) {
                TimeType recTime;
                return record && record->at( 0 )
                    && record->at( 0 )->TryCast< TimeType >( recTime )
                    && recTime == time;
            } );
            std::vector< std::shared_ptr< logika::Any > > values{};
            if ( !dataPoint->value->TryCast< std::vector< std::shared_ptr< logika::Any > > >( values ) )
            {
                LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Invalid data point value" ) );
                continue;
            }
            const size_t recordSize = 1 + values.size() * state->tvReadStates.size();
            if ( records.end() == iter || !( *iter ) )
            {
                meters::DataTable::RecordType record =
                    std::make_shared< std::vector< std::shared_ptr< logika::Any > > >( recordSize, nullptr );
                ( *record )[ 0 ] = std::make_shared< logika::Any >( time );
                row = record;
                archive->GetDataTable()->PushRecord( record );
            }
            else
            {
                row = *iter;
            }
            const size_t tvRecIdx = 1 + tv * values.size(); // Индекс начала полей для данного ТВ
            for ( size_t i = 0; i < values.size(); ++i )
            {
                ( *row )[ i + tvRecIdx ] = values.at( i );
            }
        }
    }
} // ProcessIntervalData4L


void M4Protocol::ProcessServiceData4L( std::shared_ptr< ArchiveRequestState4L > state,
    std::shared_ptr< meters::ServiceArchive > archive )
{
    if ( !state || !archive )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }
    archive->Clear();

    for ( size_t tv = 0; tv < state->tvReadStates.size(); ++tv )
    {
        TvReadState4L& trs = state->tvReadStates[ tv ];
        for ( size_t i = 0; i < trs.indices.size(); ++i )
        {
            if ( trs.fArchive )
            {
                continue;
            }
            std::shared_ptr< meters::VQT > dataPoint = trs.fArchive->GetDataPoint( trs.indices.at( i ).index );
            if ( !dataPoint || !dataPoint->value )
            {
                continue;
            }
            LocString event = LOCALIZED( "" );
            if ( !dataPoint->value->TryCast< LocString >( event ) )
            {
                LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Invalid data point value" ) );
                continue;
            }
            LocString description = LOCALIZED( "" );
            if ( StrCaseEq( trs.fArchive->GetArchiveType()->GetName(), LOCALIZED( "ErrorsLog" ) ) )
            {
                std::shared_ptr< meters::Logika4 > meter4 = std::dynamic_pointer_cast< meters::Logika4 >(
                    archive->GetMeter() );
                if ( meter4 )
                {
                    description = meter4->GetNsDescription( event );
                }
            }
            if ( state->tvReadStates.size() > 1 ) // Приборы с несколькими ТВ
            {
                event = ToLocString( std::to_string( tv + 1 ) ) + LOCALIZED( "-" ) + event;
            }
            meters::ServiceArchive::RecordType record = std::make_shared< meters::ServiceRecord >();
            record->timestamp = dataPoint->timestamp;
            record->event = event;
            record->description = description;
            archive->AddRecord( record );
        }
    }
} // ProcessServiceData4L


ArchiveId4M::Type M4Protocol::GetArchiveCode( std::shared_ptr< meters::ArchiveType > archiveType )
{
    if ( !archiveType )
    {
        throw std::invalid_argument{ "ArchiveType not set" };
    }
    if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "Hour" ) ) )
    {
        return ArchiveId4M::Hour;
    } else if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "Day" ) ) )
    {
        return ArchiveId4M::Day;
    } else if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "Month" ) ) )
    {
        return ArchiveId4M::Mon;
    } else if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "Control" ) ) )
    {
        return ArchiveId4M::Ctrl;
    }
    throw std::runtime_error{ "Unsupported archive type" };
} // GetArchiveCode


void M4Protocol::FixIntervalTimestamp( std::shared_ptr< ArchiveRecord > record,
    std::shared_ptr< meters::ArchiveType > archiveType, std::shared_ptr< M4Protocol::MeterCache > mtrInstance )
{
    if ( !record || !archiveType || !mtrInstance )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }
    if ( record->fullTime != 0 )
    {
        return;
    }

    if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "Hour" ) ) )
    {
        record->fullTime = record->timeSinceStart;
    }
    else if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "Day" ) )
        || StrCaseEq( archiveType->GetName(), LOCALIZED( "Control" ) ) )
    {
        record->fullTime = record->timeSinceStart + 60 * 60 * mtrInstance->rh_;
    }
    else if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "Month" ) ) )
    {
        record->fullTime = record->timeSinceStart
            + 60 * 60 * mtrInstance->rh_
            + 60 * 60 * 24 * ( mtrInstance->rd_ - 1 );
    }
    else
    {
        throw std::runtime_error{ "Unsupported archive type" };
    }
} // FixIntervalTimestamp


std::shared_ptr< meters::ServiceRecord > M4Protocol::ArchiveRecordToService( std::shared_ptr< meters::Logika4M > meter,
    std::shared_ptr< meters::ArchiveType > archiveType, int32_t channel,
    std::shared_ptr< ArchiveRecord > record )
{
    if ( !meter || !archiveType || !record )
    {
        throw std::invalid_argument{ "Invalid arguments" };
    }
    LocString event         = LOCALIZED( "" );
    LocString description   = LOCALIZED( "" );
    meter->GetNsDescription( event );
    if ( StrCaseEq( archiveType->GetName(), LOCALIZED( "ErrorsLog" ) ) )
    {
        description = meter->GetNsDescription( event );
    }
    if ( channel > 0 )
    {
        event = ToLocString( std::to_string( channel ) ) + LOCALIZED( "-" ) + event;
    }

    std::shared_ptr< meters::ServiceRecord > sRec = std::make_shared< meters::ServiceRecord >();
    sRec->timestamp = record->fullTime;
    sRec->event = event;
    sRec->description = description;
    return sRec;
} // ArchiveRecordToService

} // namespace M4

} // namespace protocols

} // namespace logika

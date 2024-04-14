/// @file Реализация протокола M4 (API)
/// @copyright HypeRRu 2024
/// @see m4protocol_impl.cpp

#include <logika/protocols/m4/m4protocol.h>

#include <logika/log/defines.h>
#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4l/logika4l.h>
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
    , sp_{ 0 }
    , rd_{ static_cast< MeterAddressType >( -1 ) }
    , rh_{ static_cast< MeterAddressType >( -1 ) }
    , euDict_{}
    , deviceClockDiff_{ static_cast< TimeType >( -1 ) }
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
} // GetBus


const std::shared_ptr< meters::Logika4 > M4Protocol::MeterCache::GetMeter() const
{
    return meter_;
} // GetMeter


ByteType M4Protocol::MeterCache::GetNt() const
{
    return nt_;
} // GetNt


LocString M4Protocol::MeterCache::GetModel() const
{
    if ( !model_.empty() )
    {
        return model_;
    }
    /// @todo Реализовать
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
    /// @todo Реализовать
} // LoadRdRh


TimeType M4Protocol::MeterCache::GetCurrentDeviceTime() const
{
    if ( deviceClockDiff_ != static_cast< TimeType >( -1 ) )
    {
        return GetCurrentTimestamp() - deviceClockDiff_;
    }
    /// @todo Реализовать
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

} // namespace M4

} // namespace protocols

} // namespace logika

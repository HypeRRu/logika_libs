/// @file Реализация структуры флэш архива Logika4L
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/flash_archive4l.h>

#include <logika/protocols/m4/flash_ring_buffer.h>
#include <logika/meters/types.h>
#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/meters/logika4/4l/archive_def4l.h>
#include <logika/meters/logika4/4l/archive_field_def4l.h>

/// @cond
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{

FlashArchive4::FlashArchive4( std::shared_ptr< M4Protocol::MeterCache > meterInstance,
    std::shared_ptr< meters::ArchiveDef4L > arDef,
    int32_t channelNo, MeterAddressType elementSize,
    std::shared_ptr< FlashRingBuffer::GetObjectF< TimeType > > headerTimeGetter,
    std::shared_ptr< FlashRingBuffer::GetObjectF< LocString > > headerValueGetter )
    : mtrInstance_{ meterInstance }
    , archiveDef_{ arDef }
    , headers_{}
{
    if ( !archiveDef_ )
    {
        return;
    }
    MeterAddressType idxAddr    = ( channelNo == 2 ? archiveDef_->GetIndexAddr2() : archiveDef_->GetIndexAddr() );
    MeterAddressType dataAddr   = 0x0;
    auto arType = GetArchiveType();
    if ( arType && arType->IsIntervalArchive() )
    {
        dataAddr = ( channelNo == 2 ? archiveDef_->GetHeaderAddr2() : archiveDef_->GetHeaderAddr() );
    }
    else
    {
        dataAddr = ( channelNo == 2 ? archiveDef_->GetRecordsAddr2() : archiveDef_->GetRecordsAddr() );
    }
    headers_ = std::make_shared< FlashRingBuffer >( this, idxAddr, dataAddr, archiveDef_->GetCapacity(),
        elementSize, headerTimeGetter, headerValueGetter );
} // FlashArchive4


std::shared_ptr< M4Protocol::MeterCache > FlashArchive4::GetMeterInstance() const
{
    return mtrInstance_;
} // GetMeterInstance


std::shared_ptr< meters::Logika4L > FlashArchive4::GetMeter() const
{
    return std::dynamic_pointer_cast< meters::Logika4L >( mtrInstance_ ? mtrInstance_->GetMeter() : nullptr );
} // GetMeter


std::shared_ptr< meters::ArchiveType > FlashArchive4::GetArchiveType() const
{
    return archiveDef_ ? archiveDef_->GetArchiveType() : nullptr;
} // GetArchiveType


std::shared_ptr< FlashRingBuffer > FlashArchive4::GetHeaders() const
{
    return headers_;
} // GetHeaders


void FlashArchive4::Reset()
{
    if ( headers_ )
    {
        headers_->Reset();
    }
} // Reset


std::shared_ptr< meters::VQT > FlashArchive4::GetDataPoint( uint32_t index )
{
    if ( !headers_ )
    {
        return nullptr;
    }
    TimeType nts = headers_->GetTimeElement( index );
    if ( nts == static_cast< TimeType >( -1 ) )
    {
        return nullptr;
    }

    std::shared_ptr< logika::Any > value = nullptr;
    if ( headers_->ValueGetterSet() )
    {
        value = std::make_shared< logika::Any >(
            headers_->GetValueElement( index ) );
    };
    
    std::shared_ptr< meters::VQT > hdp = std::make_shared< meters::VQT >(
        value, 0, nts
    );
    return hdp;
} // GetDataPoint


void FlashArchive4::InvalidateData( const std::vector< uint32_t >& outdated )
{
    (void) outdated;
} // InvalidateData


void FlashArchive4::UpdateData( std::vector< FrbIndex >& indicesToRead )
{
    (void) indicesToRead;
} // UpdateData

// -----------------------

SyncFlashArchive4::SyncFlashArchive4( std::shared_ptr< M4Protocol::MeterCache > meterInstance,
    std::shared_ptr< meters::ArchiveDef4L > archiveDef, int32_t channelNo )
    : FlashArchive4(
        meterInstance, archiveDef,
        channelNo, 4,
        std::make_shared< FlashRingBuffer::GetObjectF< TimeType > >( std::bind(
            &SyncFlashArchive4::GetHeaderTime,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ) ),
        nullptr
    )
    , data_{
        meterInstance,
        channelNo == 2 ? archiveDef_->GetRecordsAddr2() : archiveDef_->GetRecordsAddr(),
        archiveDef_->GetCapacity(), archiveDef_->GetRecordSize()
    }
    , rd_{ static_cast< ByteType >( meterInstance->GetRd() ) }
    , rh_{ static_cast< ByteType >( meterInstance->GetRh() ) }
    , fields_{}
{
    auto meter = meterInstance->GetMeter();
    if ( !meter || !archiveDef )
    {
        return;
    }
    for ( auto field: meter->GetArchiveFields() )
    {
        if ( field && field->GetArchiveType() == archiveDef->GetArchiveType() )
        {
            fields_.push_back( std::dynamic_pointer_cast< meters::ArchiveFieldDef4L >( field ) );
        }
    }
} // SyncFlashArchive4


ByteType SyncFlashArchive4::GetRd() const
{
    return rd_;
} // GetRd


ByteType SyncFlashArchive4::GetRh() const
{
    return rh_;
} // GetRh


std::shared_ptr< meters::VQT > SyncFlashArchive4::GetDataPoint( uint32_t index )
{
    std::shared_ptr< meters::VQT > ndhp = FlashArchive4::GetDataPoint( index );
    if ( ndhp == nullptr )
    {
        return nullptr;
    }
    ByteVector buffer;
    MeterAddressType offset;
    bool oper = false;
    data_.GetElement( index, buffer, offset );
    std::vector< std::shared_ptr< logika::Any > > varArray;
    for ( size_t i = 0; i < fields_.size(); ++i )
    {
        if ( !fields_.at( i ) )
        {
            varArray.push_back( nullptr );
            continue;
        }
        varArray.push_back(
            meters::Logika4L::GetValue( fields_.at( i )->GetInternalType(),
                buffer, offset + fields_.at( i )->GetFieldOffset(), oper )
        );
    }
    *( ndhp->value ) = varArray;
    return ndhp;
} // GetDataPoint


TimeType SyncFlashArchive4::GetHeaderTime( FlashArchive4* flashArchive,
    const ByteVector& buffer, MeterAddressType offset )
{
    constexpr TimeType invalidTime = static_cast< TimeType >( -1 );
    SyncFlashArchive4* sfa = dynamic_cast< SyncFlashArchive4* >( flashArchive );
    if ( !sfa )
    {
        return invalidTime;
    }
    return meters::Logika4L::SyncHeaderToDateTime( sfa->GetArchiveType(),
        sfa->GetRd(), sfa->GetRh(), buffer, offset );
} // GetHeaderTime


void SyncFlashArchive4::InvalidateData( const std::vector< uint32_t >& outdated )
{
    for ( uint32_t index: outdated )
    {
        data_.InvalidateElement( index );
    }
} // InvalidateData


void SyncFlashArchive4::UpdateData( std::vector< FrbIndex >& indicesToRead )
{
    data_.UpdateElements( indicesToRead );
} // UpdateData

// -----------------

AsyncFlashArchive4::AsyncFlashArchive4( std::shared_ptr< M4Protocol::MeterCache > meterInstance,
    std::shared_ptr< meters::ArchiveDef4L > archiveDef, int32_t channelNo,
    std::shared_ptr< FlashRingBuffer::GetObjectF< LocString > > valueGetter )
    : FlashArchive4(
        meterInstance, archiveDef,
        channelNo, archiveDef->GetRecordSize(),
        std::make_shared< FlashRingBuffer::GetObjectF< TimeType > >( std::bind(
            &AsyncFlashArchive4::GetAsyncRecordTime,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ) ),
        valueGetter
    )
{} // AsyncFlashArchive4


void AsyncFlashArchive4::UpdateData( std::vector< FrbIndex >& indicesToRead )
{
    indicesToRead.clear();
} // UpdateData


TimeType AsyncFlashArchive4::GetAsyncRecordTime( FlashArchive4* flashArchive,
    const ByteVector& buffer, MeterAddressType offset )
{
    (void) flashArchive;

    bool oper = false;
    return meters::Logika4L::GetSRTimestampValue( buffer, offset, oper );
} // GetAsyncRecordTime

} // namespace M4

} // namespace protocols

} // namespace logika

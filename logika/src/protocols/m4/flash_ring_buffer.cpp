/// @file Реализация структуры кольцевого буфера Flash
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/flash_ring_buffer.h>

#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/protocols/m4/m4protocol.h>

/// @cond
#include <algorithm>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{

const MeterAddressType FlashArray::PAGE_SIZE = meters::Logika4L::FLASH_PAGE_SIZE;

FlashArray::FlashArray( std::shared_ptr< M4Protocol::MeterCache > meterInstance, MeterAddressType dataAddr,
    uint32_t elementsCount, MeterAddressType elementSize )
    : mtrInstance_{ meterInstance }
    , flash_{}
    , pageMap_{}
    , dataAddress_{ dataAddr }
    , elementsCount_{ elementsCount }
    , elementSize_{ elementSize }
    , zeroPageNumber_{ 0 }
    , firstElementOffset_{ 0x0 }
{
    zeroPageNumber_     = StartPage( 0 );
    uint32_t endPage    = EndPage( elementsCount_ - 1 );
    uint32_t pageCount  = endPage - zeroPageNumber_ + 1;
    flash_              = ByteVector( pageCount * PAGE_SIZE, 0x0 );
    pageMap_            = std::vector< bool >( pageCount, false );
    firstElementOffset_ = dataAddress_ - zeroPageNumber_ * PAGE_SIZE;
} // FlashArray


uint32_t FlashArray::GetElementsCount() const
{
    return elementsCount_;
} // GetElementsCount


MeterAddressType FlashArray::GetElementSize() const
{
    return elementSize_;
} // GetElementSize


uint32_t FlashArray::StartPage( uint32_t elementIndex )
{
    return ( dataAddress_ + elementIndex * elementSize_ ) / PAGE_SIZE;
} // StartPage


uint32_t FlashArray::EndPage( uint32_t elementIndex )
{
    return ( dataAddress_ + ( elementIndex + 1 ) * elementSize_ - 1 ) / PAGE_SIZE;
} // EndPage


bool FlashArray::ElementAvailable( uint32_t index )
{
    uint32_t sp = StartPage( index );
    uint32_t ep = EndPage( index );
    for ( uint32_t p = sp; p <= ep; ++p )
    {
        if ( !pageMap_[ p - zeroPageNumber_ ] )
        {
            return false;
        }
    }
    return true;
} // ElementAvailable


void FlashArray::GetElement( uint32_t index, ByteVector& buffer, MeterAddressType& offset )
{
    if ( !ElementAvailable( index ) )
    {
        UpdateElementExplicit( index );
    }
    buffer = flash_;
    offset = firstElementOffset_ + index * elementSize_;
} // GetElement


void FlashArray::UpdatePages( uint32_t startPage, uint32_t endPage )
{
    if ( mtrInstance_ || mtrInstance_->GetBus() || endPage < startPage )
    {
        return;
    }
    uint32_t pageCount = endPage - startPage + 1;
    std::shared_ptr< meters::Logika4L > meter4L =
        std::dynamic_pointer_cast< meters::Logika4L >( mtrInstance_->GetMeter() );
    if ( !meter4L )
    {
        return;
    }
    ByteType nt = mtrInstance_->GetNt();
    ByteVector readBuf = mtrInstance_->GetBus()->ReadFlashPagesL4( meter4L,
        &nt, startPage, endPage );
    const uint32_t readPage = startPage - zeroPageNumber_;
    for ( size_t i = 0; i < readBuf.size(); ++i )
    {
        flash_[ readPage * PAGE_SIZE + i ] = readBuf.at( i );
    }
    for ( size_t i = 0; i < pageCount; ++i )
    {
        pageMap_[ readPage + i ] = true;
    }
} // UpdatePages


void FlashArray::UpdateElementExplicit( uint32_t index )
{
    UpdatePages( StartPage( index ), EndPage( index ) );
} // UpdateElementExplicit


void FlashArray::InvalidateElement( uint32_t index )
{
    uint32_t sp = StartPage( index );
    uint32_t ep = EndPage( index );
    for ( uint32_t p = sp; p <= ep; ++p )
    {
        pageMap_[ p - zeroPageNumber_ ] = false;
    }
} // InvalidateElement


void FlashArray::UpdateElements( std::vector< FrbIndex >& indices )
{
    int64_t fsp = -1;
    int64_t fep = -1;

    uint32_t i = 0;
    bool needBreak = false;
    for ( ; i < indices.size() && !needBreak; ++i )
    {
        uint32_t esp = StartPage( indices.at( i ).index ); /// element start page
        uint32_t eep = EndPage( indices.at( i ).index );   /// element end page

        for ( uint32_t p = esp; p <= eep; ++p )
        {
            if ( !pageMap_.at( p - zeroPageNumber_ ) )
            {
                if ( !ExtendPageRange( p, fsp, fep ) ) // cannot extend page range with this var
                {
                    needBreak = true;
                    break;
                }
            }
            else if ( fep != -1 && fsp != -1 )
            {
                needBreak = true; // found flash page which not needs update, and have interval to update
                break;
            }
        }
    }
    if ( fsp != -1 && fep != -1 )
    {
        UpdatePages( static_cast< uint32_t >( fsp ), static_cast< uint32_t >( fep ) );
        indices.erase( indices.begin(), indices.begin() + i );
    }
    else
    {
        indices.clear(); // no elements need an update
    }
} // UpdateElements


void FlashArray::Reset()
{
    std::fill( pageMap_.begin(), pageMap_.end(), false );
} // Reset


bool FlashArray::ExtendPageRange( uint32_t page, int64_t& startPage, int64_t& endPage )
{
    if ( startPage == -1 || endPage == -1 )
    {
        startPage   = page;
        endPage     = page;
        return true;
    }
    if ( page < startPage - 1 || page > endPage + 1 )
    {
        return false;
    }
    int64_t numSp = startPage;
    int64_t numEp = endPage;
    if ( static_cast< int64_t >( page ) == startPage
        || static_cast< int64_t >( page ) == startPage - 1 )
    {
        numSp = page;
    }
    else if ( static_cast< int64_t >( page ) == endPage
        || static_cast< int64_t >( page ) == endPage + 1 )
    {
        numEp = page;
    }
    else
    {
        return false;
    }
    if ( numEp - numSp >= M4Protocol::MAX_PAGE_BLOCK )
    {
        return false;
    }
    startPage   = numSp;
    endPage     = numEp;
    return true;
} // ExtendPageRange

// -----------------------

// void FlashRingBuffer::Reset()
// {
//     FlashArray::Reset();
//     /// @todo реализовать
// } // Reset

// -----------------------

FrbIndex::FrbIndex( uint32_t idx, TimeType t )
    : index{ idx }
    , time{ t }
{} // FrbIndex


LocString FrbIndex::ToString() const
{
    return ToLocString( std::to_string( index ) ) + LOCALIZED( ": " ) + GetTimeString( time );
} // ToString


int32_t FrbIndex::CompareByIdx( const FrbIndex& lhs, const FrbIndex& rhs )
{
    return lhs.index - rhs.index;
} // CompareByIdx

} // namespace M4

} // namespace protocols

} // namespace logika

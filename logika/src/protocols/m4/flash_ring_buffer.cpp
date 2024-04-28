/// @file Реализация структуры кольцевого буфера Flash
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/flash_ring_buffer.h>

#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4l/logika4l.h>
#include <logika/protocols/m4/m4protocol.h>
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


std::shared_ptr< M4Protocol::MeterCache > FlashArray::GetMeterInstance() const
{
    return mtrInstance_;
} // GetMeterInstance


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
    if ( !mtrInstance_ || !mtrInstance_->GetBus() || endPage < startPage )
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

FlashRingBuffer::FlashRingBuffer( FlashArchive4* parent, MeterAddressType indexAddress, MeterAddressType dataAddress,
    uint32_t elementsCount, MeterAddressType elementSize,
    std::shared_ptr< GetObjectF< TimeType > > headerTimeGetter,
    std::shared_ptr< GetObjectF< LocString > > headerValueGetter )
    : FlashArray( parent ? parent->GetMeterInstance() : nullptr, dataAddress, elementsCount, elementSize )
    , prevIdx_{ -1 }
    , prevIdxTimestamp_{ 0 }
    , prevReadDevTime_{ 0 }
    , parentArchive_{ parent }
    , indexAddress_{ indexAddress }
    , timeGetter_{ headerTimeGetter }
    , valueGetter_{ headerValueGetter }
{} // FlashRingBuffer


void FlashRingBuffer::Reset()
{
    FlashArray::Reset();
    prevIdx_            = -1;
    prevIdxTimestamp_   =  0;
    prevReadDevTime_    =  0;
} // Reset


MeterAddressType FlashRingBuffer::GetIndexAddress() const
{
    return indexAddress_;
} // GetIndexAddress


TimeType FlashRingBuffer::GetTimeElement( uint32_t index )
{
    ByteVector buffer;
    MeterAddressType offset;
    if ( !parentArchive_ || !timeGetter_ )
    {
        return static_cast< TimeType >( -1 );
    }
    GetElement( index, buffer, offset );
    return ( *timeGetter_ )( parentArchive_, buffer, offset );
} // GetTimeElement


LocString FlashRingBuffer::GetValueElement( uint32_t index )
{
    ByteVector buffer;
    MeterAddressType offset;
    if ( !parentArchive_ || !valueGetter_ )
    {
        return LOCALIZED( "" );
    }
    GetElement( index, buffer, offset );
    return ( *valueGetter_ )( parentArchive_, buffer, offset );
} // GetValueElement


bool FlashRingBuffer::TimeGetterSet() const
{
    return timeGetter_ != nullptr;
} // TimeGetterSet


bool FlashRingBuffer::ValueGetterSet() const
{
    return valueGetter_ != nullptr;
} // ValueGetterSet


bool FlashRingBuffer::GetElementIndicesInRange( TimeType startTime, TimeType endTime, uint32_t lastWrittenIndex,
    int64_t& restartPoint, std::vector< FrbIndex >& indices, double& percentCompleted )
{
    if ( restartPoint < 0 )
    {
        restartPoint = lastWrittenIndex;
    }
    bool finished = false;
    uint32_t readsDone = 0;
    uint32_t count = ( elementsCount_ + restartPoint - lastWrittenIndex ) % elementsCount_;
    if ( count == 0 )
    {
        count = elementsCount_;
    }
    uint32_t current = restartPoint;
    for ( uint32_t i = 0; i < count; ++i ) /// Проход по всем (максимальному количеству) элементов в буфере
    {
        current = ( elementsCount_ + restartPoint - i ) % elementsCount_;
        if ( ElementAvailable( current ) )
        {
            TimeType time = GetTimeElement( current );
            if ( static_cast< TimeType >( -1 ) == time )
            {
                /// В СПГ741(как минимум) забавный глюк - последний эл-т часовых индексов не пишется (==0x00000000)
                if ( current == elementsCount_ - 1 ) /// Поэтому не заканчиваем обработку если пустой индекс находится на границе буфера.
                {
                    continue;
                }
                else
                {
                    finished = true; /// Достигнут не инициализированный/очищенный элемент (достигнуто начало кольцевого буфера)
                    break; /// Дальнейшая обработка не требуется
                }
            }
            if ( 0 == time )
            {
                /// Пропуск элементов с невалидными метками времени
                continue;
            }

            if ( time >= startTime && time <= endTime )
            {
                indices.push_back( FrbIndex{ current, time } );
            }

            finished = ( time <= startTime );
            finished = finished || ( i == count - 1 );
            if ( finished )
            {
                break;
            }
        }
        else
        {
            /// cache miss - scan for update elements from [ci to endIndex]
            //не запрашиваем за один выделенный квант времени больше одной пачки страниц
            if ( readsDone > 0 )
            {
                break;
            }
            uint32_t elementsLeft = count - i;

            int64_t fsp = -1;
            int64_t fep = -1;
            bool needBreak = false;
            for ( uint32_t t = 0; t < elementsLeft && !needBreak; ++t )
            {
                uint32_t tCurr = ( elementsCount_ + current - t ) % elementsCount_;
            
                uint32_t esp = StartPage( tCurr ); // element start page 
                uint32_t eep = EndPage( tCurr );   // element end page 
            
                // Если элементы будут занимать > 2 стр -> группировка не всегда будет работать 
                // (нужно будет менять направление цикла в зависимости от direction)
                for ( uint32_t page = eep; page >= esp; --page )
                {
                    if ( !pageMap_.at( page - zeroPageNumber_ ) )
                    {
                        if ( !ExtendPageRange( page, fsp, fep ) )
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

            UpdatePages( fsp, fep );
            ++readsDone;
            --i; // Возвращаем обработку на предыдущий элемент (иначе будет пропущен стартовый)
        }
    }

    restartPoint = current;
    percentCompleted = indices.size() * 100.0 / elementsCount_;

    if ( finished )
    {
        std::reverse( indices.begin(), indices.end() );
        percentCompleted = 100.0;
    }

    return finished;
} // GetElementIndicesInRange


void FlashRingBuffer::ManageOutdatedElements( bool useIndexCache,
    std::vector< uint32_t >& newIndices, uint32_t& currentIndex )
{
    constexpr TimeType gT = 15; ///< Погрешность

    std::vector< uint32_t > outdated{};
    // Возможна ситуация, когда прочитанный индекс относится по нашему мнению к новому часу, а в приборе он еще не обновился,
    // плюс есть небольшая ошибка в определении разницы времени прибора и сервера
    // => весь следующий час сервер будет использовать неправильно закешированный старый индекс.
    // поэтому: рядом с границей часа индекс читаем, используем, но не кешируем
    if ( !parentArchive_ )
    {
        return;
    }
    auto mtrInstance = parentArchive_->GetMeterInstance();
    if ( !mtrInstance )
    {
        return;
    }
    TimeType currentDeviceTime = mtrInstance->GetCurrentDeviceTime();
    struct tm devTs = GetTimeStruct( currentDeviceTime );
    bool atGuardInterval = ( devTs.tm_min == 59 && static_cast< TimeType >( devTs.tm_sec ) > 60 - gT )
        || ( devTs.tm_min == 0 && static_cast< TimeType >( devTs.tm_sec ) < gT );
    if ( useIndexCache && prevIdx_ != -1 && prevIdxTimestamp_ != 0 )
    {
        struct tm prevTs = GetTimeStruct( prevIdxTimestamp_ );
        if (   prevTs.tm_year == devTs.tm_year
            && prevTs.tm_mon  == devTs.tm_mon
            && prevTs.tm_mday == devTs.tm_mday
            && prevTs.tm_hour == devTs.tm_hour )
        {
            currentIndex = prevIdx_;
            newIndices = outdated;
            return;
        }
    }

    auto bus = mtrInstance->GetBus();
    std::shared_ptr< meters::Logika4L > meter4L =
        std::dynamic_pointer_cast< meters::Logika4L >( mtrInstance->GetMeter() );
    if ( !bus || !meter4L )
    {
        return;
    }
    ByteType nt = mtrInstance->GetNt();
    ByteVector iBytes = bus->ReadFlashPagesL4( meter4L, &nt, indexAddress_, sizeof( uint16_t ) );
    currentIndex = ( iBytes[ 1 ] << 8 ) | iBytes[ 0 ];
    if ( currentIndex >= elementsCount_ )
    {
        throw std::runtime_error{ "Invalid end of archive pointer: " + std::to_string( currentIndex ) };
    }
    if ( prevIdx_ != -1 )
    {
        UpdateElementExplicit( static_cast< uint32_t >( prevIdx_ ) ); /// Чтение без кэша
        TimeType prevActualTimeStamp = GetTimeElement( prevIdx_ );

        uint32_t start = 0;
        uint32_t count = 0;
        // Если время элемента по запомненному указателю изменилось -
        // считаем что все кольцо буфера надо читать заново
        bool invalidateAll = ( prevIdxTimestamp_ != prevActualTimeStamp );
        if ( invalidateAll )
        {
            start = 0;
            count = elementsCount_;
        }
        else if ( prevIdx_ != currentIndex ) // if ptr shifted, invalidate only [prev .. curr] elements  
        {
            start = prevIdx_;
            count = ( elementsCount_ + currentIndex - prevIdx_ ) % elementsCount_;
        }
        if ( count != 0 )
        {
            for ( uint32_t i = 0; i < count; ++i )
            {
                uint32_t idx = ( start + i ) % elementsCount_;
                InvalidateElement( idx );
                outdated.push_back( idx );
            }
        }
    }

    prevIdx_ = currentIndex;
    if ( !atGuardInterval )
    {
        prevReadDevTime_ = mtrInstance->GetCurrentDeviceTime();
    }
    else
    {
        prevReadDevTime_ = 0; // не кешируем индекс, прочитанный в защитном интервале
    }
    prevIdxTimestamp_ = GetTimeElement( currentIndex );
    newIndices = outdated;
} // ManageOutdatedElements

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


bool FrbIndex::LessByIdx( const FrbIndex& lhs, const FrbIndex& rhs )
{
    return CompareByIdx( lhs, rhs ) < 0;
} // LessByIdx

} // namespace M4

} // namespace protocols

} // namespace logika

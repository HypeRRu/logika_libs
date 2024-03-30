/// @file Реализация сервисного архива
/// @copyright HypeRRu 2024

#include <logika/meters/service_archive.h>

#include <logika/common/misc.h>

/// @cond
#include <stdexcept>
#include <algorithm>
/// @endcond

namespace logika
{

namespace meters
{

/// Запись сервисного архива

std::string ServiceRecord::ToString() const
{
    return std::to_string( timestamp ) + " " + event + " " + description;
} // ToString


/// Сервисный архив

ServiceArchive::ServiceArchive( std::shared_ptr< IMeter > meter, std::shared_ptr< ArchiveType > archiveType )
    : Archive( meter, archiveType )
    , records_{}
{
    if ( !archiveType || !archiveType->IsServiceArchive() )
    {
        throw std::runtime_error{ "Wrong archive type" };
    }
} // ServiceArchive


const std::vector< ServiceArchive::RecordType >& ServiceArchive::GetRecords() const
{
    return records_;
} // GetRecords


void ServiceArchive::AddRecord( ServiceArchive::RecordType record )
{
    auto iter = records_.begin();
    for ( ; iter != records_.end(); ++iter )
    {
        ServiceArchive::RecordType r = *iter;
        if ( !r )
        {
            continue;
        }
        if ( r->timestamp > record->timestamp )
        {
            break;
        }
    }
    records_.insert( iter, record );
} // AddRecord


const ServiceArchive::RecordType ServiceArchive::GetRecord( size_t index ) const
{
    if ( index >= records_.size() )
    {
        return nullptr;
    }
    return records_.at( index );
} // GetRecord


const ServiceArchive::RecordType ServiceArchive::GetRecordByTimestamp( TimeType timestamp ) const
{
    auto iter = std::find_if( records_.cbegin(), records_.cend(), [ timestamp ]( const ServiceArchive::RecordType& r ) {
        if ( !r )
        {
            return false;
        }
        return r->timestamp == timestamp;
    } );
    if ( records_.cend() == iter )
    {
        return nullptr;
    }
    return *iter;
} // GetRecordByTimestamp


bool ServiceArchive::RemoveRecord( size_t index )
{
    if ( index >= records_.size() )
    {
        return false;
    }
    records_.erase( records_.begin() + index );
    return true;
} // RemoveRecord( size_t index )


bool ServiceArchive::RemoveRecord( ServiceArchive::RecordType record )
{
    if ( !record )
    {
        return false;
    }
    auto iter = std::find_if( records_.cbegin(), records_.cend(), [ record ]( const ServiceArchive::RecordType& r ) {
        if ( !r )
        {
            return false;
        }
        return r == record
            || ( r->timestamp   == record->timestamp
            &&   r->event       == record->event
            &&   r->description == record->description );
    } );
    if ( records_.cend() == iter )
    {
        return false;
    }
    records_.erase( iter );
    return true;
} // RemoveRecord( RecordType record )


bool ServiceArchive::RemoveRecordByTimestamp( logika::TimeType timestamp )
{
    auto iter = std::find_if( records_.cbegin(), records_.cend(), [ timestamp ]( const ServiceArchive::RecordType& r ) {
        if ( !r )
        {
            return false;
        }
        return r->timestamp == timestamp;
    } );
    if ( records_.cend() == iter )
    {
        return false;
    }
    records_.erase( iter );
    return true;
} // RemoveRecordByTimestamp

} // namespace meters

} // namespace logika

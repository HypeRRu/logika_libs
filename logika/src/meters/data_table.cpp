/// @file Реализация таблицы данных
/// @copyright HypeRRu 2024

#include <logika/meters/data_table.h>

#include <stdexcept>
#include <algorithm>

namespace logika
{

namespace meters
{

DataTable::DataTable( const std::vector< FieldType >& fieldList )
    : fieldList_{ fieldList }
    , data_{}
{} // DataTable


size_t DataTable::RecordCount() const
{
    return data_.size();
} // RecordCount


size_t DataTable::FieldCount() const
{
    return fieldList_.size();
} // FieldCount


const DataTable::RecordType DataTable::GetRecord( size_t index ) const
{
    if ( index >= data_.size() )
    {
        return nullptr;
    }
    return data_.at( index );
} // GetRecord


const DataTable::RecordType DataTable::PopRecord()
{
    if ( data_.empty() )
    {
        return nullptr;
    }
    const RecordType record = data_.back();
    data_.pop_back();
    return record;
} // PopRecord


bool DataTable::RemoveRecord( DataTable::RecordType record )
{
    auto iter = std::find( data_.cbegin(), data_.cend(), record );
    if ( data_.cend() == iter )
    {
        return false;
    }
    data_.erase( iter );
    return true;
} // RemoveRecord( RecordType record )


bool DataTable::RemoveRecord( size_t index )
{
    if ( index >= data_.size() )
    {
        return false;
    }
    data_.erase( data_.begin() + index );
    return true;
} // RemoveRecord( size_t index )


bool DataTable::PushRecord( DataTable::RecordType record )
{
    if ( !ValidateRecord( record ) )
    {
        return false;
    }
    data_.push_back( record );
    return true;
} // PushRecord


bool DataTable::InsertRecord( DataTable::RecordType record, size_t index )
{
    if ( index > data_.size() || !ValidateRecord( record ) )
    {
        return false;
    }
    data_.insert( data_.begin() + index, record );
    return true;
} // InsertRecord


bool DataTable::ValidateRecord( DataTable::RecordType record )
{
    /// @todo реализовать
    return true;
} // ValidateRecord

} // namespace meters

} // namespace logika

/// @file Реализация таблицы данных
/// @copyright HypeRRu 2024

#include <logika/meters/data_table.hpp>

/// @cond
#include <stdexcept>
#include <algorithm>
/// @endcond

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


const std::vector< DataTable::FieldType >& DataTable::GetFieldsList() const
{
    return fieldList_;
} // GetFieldsList


const std::vector< DataTable::RecordType >& DataTable::GetRecordsList() const
{
    return data_;
} // GetRecordsList


std::vector< DataTable::RecordType >& DataTable::GetRecordsList()
{
    return data_;
} // GetRecordsList


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



bool DataTable::ValidateField( DataTable::FieldType field )
{
    /// @todo реализовать
    return true;
} // ValidateField


bool DataTable::AddColumn( DataTable::FieldType field )
{
    if ( !ValidateField( field ) )
    {
        return false;
    }

    fieldList_.push_back( field );
    for ( RecordType record: data_ )
    {
        if ( record )
        {
            record->push_back( std::make_shared< logika::Any >() );
        }
    }
    return true;
} // AddColumn


bool DataTable::InsertColumn( DataTable::FieldType field, size_t index )
{
    if ( !ValidateField( field ) )
    {
        return false;
    }

    fieldList_.insert( fieldList_.begin() + index, field );
    for ( RecordType record: data_ )
    {
        if ( record )
        {
            record->insert( record->begin() + index, std::make_shared< logika::Any >() );
        }
    }

    return true;
} // InsertColumn


bool DataTable::RemoveColumn( DataTable::FieldType field )
{
    size_t index = 0;
    if ( !GetColumnIndex( field, index ) )
    {
        return {};
    }
    return RemoveColumn( index );
} // RemoveColumn


bool DataTable::RemoveColumn( size_t fieldIndex )
{
    if ( fieldIndex >= fieldList_.size() )
    {
        return false;
    }
    fieldList_.erase( fieldList_.begin() + fieldIndex );
    for ( DataTable::RecordType record: data_ )
    {
        if ( record )
        {
            record->erase( record->begin() + fieldIndex );
        }
    }
    return true;
} // RemoveColumn


bool DataTable::GetColumnIndex( FieldType field, size_t& index )
{
    auto iter = std::find( fieldList_.cbegin(), fieldList_.cend(), field );
    if ( fieldList_.cend() == iter )
    {
        return false;
    }
    index = iter - fieldList_.cbegin();
    return true;
} // GetColumnIndex


std::vector< std::shared_ptr< logika::Any > > DataTable::GetColumn( FieldType field )
{
    size_t index = 0;
    if ( !GetColumnIndex( field, index ) )
    {
        return {};
    }
    return GetColumn( index );
} // GetColumn( FieldType )


std::vector< std::shared_ptr< logika::Any > > DataTable::GetColumn( size_t fieldIndex )
{
    if ( fieldIndex >= fieldList_.size() )
    {
        return {};
    }
    std::vector< std::shared_ptr< logika::Any > > column{};
    column.reserve( data_.size() );
    for ( auto record: data_ )
    {
        if ( !record )
        {
            continue;
        }
        column.emplace_back( record->at( fieldIndex ) );
    }
    return column;
} // GetColumn( size_t )


void DataTable::Clear()
{
    data_.clear();
} // Clear

} // namespace meters

} // namespace logika

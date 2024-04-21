/// @file Реализация шаблонных методов таблицы данных
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TABLE_IPP
#define LOGIKA_METERS_DATA_TABLE_IPP

#include <logika/meters/data_table.h>

/// @cond
#include <stdexcept>
/// @endcond

namespace logika
{

namespace meters
{

template < typename T >
std::vector< T > DataTable::GetColumn( FieldType field ) const
{
    auto fieldEntry = fieldList_.find( field );
    if ( fieldList_.cend() == fieldEntry )
    {
        throw std::out_of_range{ "Field not found" };
    }
    return GetColumn< T >( fieldEntry - fieldList_.cbegin() );
} // GetColumn( FieldType field )


template < typename T >
std::vector< T > DataTable::GetColumn( size_t fieldIndex ) const
{
    if ( fieldIndex >= fieldList_.size() )
    {
        throw std::out_of_range{ "Field index out of bounds" };
    }
    std::vector< T > column;
    column.reserve( data_.size() );

    for ( RecordType record: data_ )
    {
        column.push_back( record->at( fieldIndex ).Cast< T > );
    }
    return column;
} // GetColumn( size_t fieldIndex )


template < typename T >
bool DataTable::AddColumn( DataTable::FieldType field, const T& value )
{
    if ( !AddColumn( field ) )
    {
        return false;
    }
    for ( DataTable::RecordType record: data_ )
    {
        if ( record )
        {
            record->back() = std::make_shared< logika::Any >( value );
        }
    }
    return true;
} // AddColumn( DataTable::FieldType field, const T& value )


template < typename T >
bool DataTable::InsertColumn( DataTable::FieldType field, size_t index, const T& value )
{
    if ( !InsertColumn( field, index ) )
    {
        return false;
    }
    for ( DataTable::RecordType record: data_ )
    {
        if ( record )
        {
            record[ index ] = std::make_shared< logika::Any >( value );
        }
    }
    return true;
} // InsertColumn( DataTable::FieldType field, size_t index, const T& value )

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_DATA_TABLE_IPP

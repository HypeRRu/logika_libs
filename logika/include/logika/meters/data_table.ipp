/// @file Реализация шаблонных методов таблицы данных
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TABLE_IPP
#define LOGIKA_METERS_DATA_TABLE_IPP

#include <logika/meters/data_table.h>

#include <stdexcept>

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
        column.push_back( record.at( fieldIndex ).Cast< T > );
    }
    return column;
} // GetColumn( size_t fieldIndex )

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_DATA_TABLE_IPP

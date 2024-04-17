/// @file Реализация шаблонных методов тэга с данными
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TAG_IPP
#define LOGIKA_METERS_DATA_TAG_IPP

#include <logika/meters/data_tag.h>

namespace logika
{

namespace meters
{

template < typename T >
T DataTag::GetValue() const
{
    if ( !value_ )
    {
        throw std::runtime_error{ "Value not set" };
    }
    return value_->Cast< T >();
} // GetValue


template < typename T >
bool DataTag::TryGetValue( T& value ) const
{
    return value_ && value_->TryCast< T >( value );
} // TryGetValue


template < typename T >
void DataTag::SetValue( const T& value )
{
    if ( !value_ )
    {
        value_ = std::make_shared< logika::Any >( value );
        return;
    }
    *value_ = value;
} // SetValue

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_DATA_TAG_IPP
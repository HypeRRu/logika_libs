/// @file Реализация объекта переменного типа
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_ANY_IPP
#define LOGIKA_ANY_IPP

#include <logika/common/any.hpp>

/// @cond
#include <stdexcept>
/// @endcond

namespace logika
{

template < typename T >
Any::Any( const T& t )
    : holder_{ new Holder< T >( t ) }
{} // Any( const T& t )


template < typename T >
Any& Any::operator= ( const T& value )
{
    holder_ = new Holder< T >( value );
    return *this;
} // operator= ( const T& value )


template < typename T >
Any& Any::operator= ( T&& value )
{
    holder_ = new Holder< T >( value );
    return *this;
} // operator= ( T&& value )


template < typename T >
T Any::Cast() const
{
    if ( Empty() )
    {
        throw std::runtime_error{ "Attempt to cast empty any-value" };
    }
    if ( typeid( T ) != holder_->TypeInfo() )
    {
        throw std::runtime_error{ "Bad any cast" };
    }
    return static_cast< Holder< T >* >( holder_.get() )->value;
} // Cast


template < typename T >
Any::Holder< T >::Holder( const T& t )
    : value{ t }
{} // Holder( const T& t )


template < typename T >
const std::type_info& Any::Holder< T >::TypeInfo() const
{
    return typeid( T );
} // TypeInfo

} // namespace logika

#endif // LOGIKA_ANY_IPP

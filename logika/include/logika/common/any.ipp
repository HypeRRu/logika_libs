/// @file Реализация объекта переменного типа
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_ANY_IPP
#define LOGIKA_ANY_IPP

#include <logika/common/any.hpp>

#include <logika/common/misc.h>

/// @cond
#include <stdexcept>

// #ifdef __GNUC__
// #include <cstdlib>
// #include <cxxabi.h> // for demangle
// #endif // __GNUC__
/// @endcond

namespace // anonymous
{

// /// @brief Получение строкового представления имени типа
// /// @param[in] typeInfo Информация о типе
// /// @return Строковое представление имени типа
// std::string GetTypeName( const std::type_info& typeInfo )
// {
// #if defined( __GNUC__ )
//     int     status = 0;
//     char*   realname = nullptr;
//     realname = abi::__cxa_demangle( typeInfo.name(), nullptr, nullptr, &status );
//     if ( 0 == status )
//     {
//         std::string typeName{ realname };
//         ::free( realname );
//         return typeName;
//     }
//     return typeInfo.name();
// #else // ! defined( __GNUC__ )
//     return typeInfo.name();
// #endif // defined( __GNUC__ )
// } // GetTypeName

} // anonymous namespace


namespace logika
{

template < typename T >
Any::Any( const T& t )
    : holder_{ new Holder< T >( t ) }
{} // Any( const T& t )


template < typename T >
Any& Any::operator= ( const T& value )
{
    holder_ = std::unique_ptr< Holder< T > >( new Holder< T >( value ) );
    return *this;
} // operator= ( const T& value )


template < typename T >
Any& Any::operator= ( T&& value )
{
    holder_ = std::unique_ptr< Holder< T > >( new Holder< T >( value ) );
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
bool Any::TryCast( T& result ) const
{
    if ( Empty() )
    {
        return false;
    }
    if ( typeid( T ) != holder_->TypeInfo() )
    {
        return false;
    }
    result = static_cast< Holder< T >* >( holder_.get() )->value;
    return true;
} // TryCast


template < typename T >
bool Any::CheckType() const
{
    if ( Empty() )
    {
        return true;
    }
    return typeid( T ) == holder_->TypeInfo();
} // CheckType


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

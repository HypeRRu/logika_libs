/// @file Описание конструироемого в shared_ptr типа
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_SHARED_CONSTRUCTIBLE_HPP
#define LOGIKA_SHARED_CONSTRUCTIBLE_HPP

/// @cond
#include <memory>
#include <type_traits>
/// @endcond

namespace logika
{

/// @brief Конструируемый в shared_ptr тип
/// @tparam Type Тип возвращаемого значения
template < typename Type >
class SharedConstructible
{
public:
    virtual ~SharedConstructible() = default;

    /// @brief Создание shared_ptr на объект указанного типа Type
    /// @tparam ConstructedType Реальный тип конструируемого объекта
    /// @tparam Args Типы аргументов для создания объекта
    /// @param[in] args Аргументы для создания объекта
    /// @return shared_ptr на созданный объект
    /// @note ConstructedType должен быть полиморфен Type
    template < typename ConstructedType, typename... Args >
    static std::shared_ptr< Type > Create( Args&&... args )
    {
        struct MakeSharedEnabler: public ConstructedType
        {
        public:
            MakeSharedEnabler( Args&&... args )
                : ConstructedType( std::forward< Args >( args )... )
            {}

        };
        return std::make_shared< MakeSharedEnabler >( std::forward< Args >( args )... );
    } // Create

}; // class SharedConstructible

} // namespace logika

#endif // LOGIKA_SHARED_CONSTRUCTIBLE_HPP

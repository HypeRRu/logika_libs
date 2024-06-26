/// @file Описание объекта переменного типа
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_ANY_HPP
#define LOGIKA_ANY_HPP

#include <logika/common/defs.h>

/// @cond
#include <typeinfo>
#include <memory>
/// @endcond

namespace logika
{

/// @brief Объект переменного типа
/// @note Тип сохраняемого объекта должен быть copy-constructible
class LOGIKA_COMMON_EXPORT Any
{
public:
    /// @brief Конструктор объекта переменного типа
    /// @tparam T тип объекта
    /// @param[in] t Значение объекта
    template < typename T >
    Any( const T& t );

    Any()  = default;
    ~Any() = default;

    /// @brief Конструктор перемещения
    Any( Any&& other );

    /// @brief Оператор перемещения
    /// @param[in] other Перемещаемый объект переменного типа
    Any& operator= ( Any&& other );

    /// @brief Оператор копирования
    /// @tparam Тип копируемого объекта
    /// @param[in] value Копируемый объект
    template < typename T >
    Any& operator= ( const T& value );

    /// @brief Оператор перемещения
    /// @tparam Тип копируемого объекта
    /// @param[in] value Перемещаемый объект
    template < typename T >
    Any& operator= ( T&& value );

    /// @brief Задано ли значение объекта
    /// @return Было ли установлено значение объекта
    bool Empty() const;

    /// @brief Очистка значения объекта
    void Erase();

    /// @brief Получение значения объекта
    /// @tparam T Тип получаемого объекта
    /// @return Хранимый объект
    /// @throws std::runtime_error Если значение не установлено или типы не совпадают
    template < typename T >
    T Cast() const;

    /// @brief Попытка получения значения объекта
    /// @tparam T Тип получаемого объекта
    /// @param[out] result Хранимый объект
    /// @return true - если удалось извлечь хранимый объект, false - иначе
    template < typename T >
    bool TryCast( T& result ) const;

    /// @brief Проверка типа переменной
    /// @tparam T Проверяемый тип
    /// @return Совпадает ли тип T с типом сохраненного значения
    template < typename T >
    bool CheckType() const;

private:
    /// @brief Базовая вспомогательная структура для хранения значения
    struct BaseHolder
    {
    public:
        virtual ~BaseHolder() = default;

        /// @brief Получение информации о типе
        /// @return Информация о типе
        virtual const std::type_info& TypeInfo() const = 0;
    }; // struct BaseHolder

    /// @brief Типизированная вспомогательная структура для хранения значения
    template < typename T >
    struct Holder: public BaseHolder
    {
    public:
        /// @brief Конструктор "хранителя"
        /// @param[in] t Сохраняемое значение
        Holder( const T& t );

        /// @copydoc BaseHolder::TypeInfo()
        const std::type_info& TypeInfo() const override;

    public:
        T value;    ///< Сохраняемое значение

    }; // struct Holder

private:
    std::unique_ptr< BaseHolder > holder_;  ///< "Хранитель" значения

}; // class Any

} // namespace logika

#include <logika/common/any.ipp>

#endif // LOGIKA_ANY_HPP

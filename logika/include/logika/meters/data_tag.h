/// @file Определение тэга с данными
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TAG_H
#define LOGIKA_METERS_DATA_TAG_H

#include <logika/meters/defs.h>
#include <logika/meters/tag.h>
#include <logika/meters/data_tag_def.h>
#include <logika/common/any.hpp>

/// @cond
#include <string>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Тэг с данными
class LOGIKA_METERS_EXPORT DataTag: public Tag
{
public:
    /// @brief Конструктор тэга с данными
    /// @param[in] def Описание тэга
    /// @param[in] channelNo Номер канала
    DataTag( std::shared_ptr< DataTagDef > def, int32_t channelNo );

    /// @brief Получение данных тэга
    /// @tparam T Тип данных тэга
    /// @return Данные тэга
    /// @throws std::runtime_error Если указан неверный тип данных
    template < typename T >
    T GetValue() const;

    /// @brief Попытка получения данных тэга
    /// @tparam T Тип данных тэга
    /// @return Данные тэга
    /// @return false если указан неверный тип данных, true - если удалось получить данные
    template < typename T >
    bool TryGetValue( T& value ) const;

    /// @brief Запись данных тэга
    /// @tparam Тип данных тэга
    /// @param[in] value Данные тэга
    template < typename T >
    void SetValue( const T& value );

    /// @brief Запись данных тэга
    /// @param[in] value Новое значение тэга
    void ReplaceValue( std::shared_ptr< logika::Any > value );

    /// @brief Содержит ли тэг данные
    /// @return Наличие данных в тэге
    bool HasValue() const;

    /// @brief Получение индекса элемента
    /// @return Индекс элемента или -1, если не задан
    int32_t GetIndex() const;

    /// @brief Получение метки времени
    /// @return Метка времени (мс)
    TimeType GetTimestamp() const;

    /// @brief Установка метки времени
    /// @param[in] stamp Метка времени (мс)
    void SetTimestamp( TimeType stamp );

    /// @brief Получение флага операции
    /// @return Флаг операции
    bool GetOper() const;

    /// @brief Установка флага операции
    /// @param[in] oper Флаг операции
    void SetOper( bool oper );

    /// @brief Получение описания ошибки
    /// @return Описание ошибки
    LocString GetErrorDescription() const;

    /// @brief Установка описания ошибки
    /// @param[in] description Описание ошибки
    void SetErrorDescription( const LocString& description );

    /// @brief Получение формата отображения
    /// @return Формат отображения
    LocString GetDisplayFormat() const;

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

protected:
    std::shared_ptr< logika::Any > value_;      ///< Данные тэга
    TimeType timestamp_;                        ///< Метка времени (мс)
    bool oper_;                                 ///< Флаг операции
    LocString errDesc_;                         ///< Описание ошибки
    std::shared_ptr< DataTagDef > dataTagDef_;  ///< Описание тэга

}; // class DataTag

} // namespace meters

} // namespace logika

#include <logika/meters/data_tag.ipp>

#endif // LOGIKA_METERS_DATA_TAG_H

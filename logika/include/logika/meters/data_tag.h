/// @file Определение тэга с данными
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TAG_H
#define LOGIKA_METERS_DATA_TAG_H

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
class DataTag: public Tag
{
public:
    /// @brief Конструктор тэга с данными
    /// @param[in] def Описание тэга
    /// @param[in] channelNo Номер канала
    DataTag( const DataTagDef& def, int32_t channelNo );

    /// @brief Получение данных тэга
    /// @tparam T Тип данных тэга
    /// @return Данные тэга
    /// @throws std::runtime_error Если указан неверный тип данных
    template < typename T >
    T GetValue() const;

    /// @brief Запись данных тэга
    /// @tparam Тип данных тэга
    /// @param[in] value Данные тэга
    template < typename T >
    void SetValue( const T& value );

    /// @brief Получение индекса элемента
    /// @return Индекс элемента или -1, если не задан
    int32_t GetIndex() const;

    /// @brief Получение метки времени
    /// @return Метка времени
    TimeType GetTimestamp() const;

    /// @brief Установка метки времени
    /// @param[in] stamp Метка времени
    void SetTimestamp( TimeType stamp );

    /// @brief Получение флага операции
    /// @return Флаг операции
    bool GetOper() const;

    /// @brief Установка флага операции
    /// @param[in] oper Флаг операции
    void SetOper( bool oper );

    /// @brief Получение описания ошибки
    /// @return Описание ошибки
    std::string GetErrorDescription() const;

    /// @brief Установка описания ошибки
    /// @param[in] description Описание ошибки
    void SetErrorDescription( const std::string& description );

    /// @brief Получение формата отображения
    /// @return Формат отображения
    std::string GetDisplayFormat() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

protected:
    logika::Any value_;         ///< Данные тэга
    TimeType timestamp_;        ///< Метка времени
    bool oper_;                 ///< Флаг операции
    std::string errDesc_;       ///< Описание ошибки
    DataTagDef dataTagDef_;     ///< Описание тэга

}; // class DataTag

} // namespace meters

} // namespace logika

#include <logika/meters/data_tag.ipp>

#endif // LOGIKA_METERS_DATA_TAG_H

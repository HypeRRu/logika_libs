/// @file Определение описания тэга с данными
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TAG_DEF_H
#define LOGIKA_METERS_DATA_TAG_DEF_H

#include <logika/meters/tag_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания тэга с данными
struct DataTagDefSettings: public TagDefSettings
{
public:
    TagKind::Type kind = TagKind::Undefined;    ///< Тип тэга
    const std::string descriptionEx = "";       ///< Расширенное описание (для настроечных параметров)
    const std::string range = "";               ///< Диапазон (для настроечных параметров)
    bool isBasicParam = true;                   ///< Является ли параметр базовым
    uint32_t updateRate = 0;                    ///< Частота обновления параметра

}; // struct DataTagDefSettings


/// @brief Описание тэга с данными
class DataTagDef: public TagDef, public ISerializable
{
public:
    /// @brief Конструктор тэга с данными
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга с данными
    DataTagDef( const ChannelDef& cdef, const DataTagDefSettings& settings );
    
    /// @brief Получение типа тэга
    /// @return Тип тэга
    TagKind::Type GetKind() const;

    /// @brief Получение расширенного описания
    /// @return Расширенное описание
    std::string GetDescriptionEx() const;

    /// @brief Получение диапазона
    /// @return Диапазон
    std::string GetRange() const;

    /// @brief Получение флага базового параметра
    /// @return Является ли параметр базовым
    bool IsBasicParam() const;

    /// @brief Получение частоты обновления
    /// @return Частота обновления
    uint32_t GetUpdateRate() const;

    /// @brief Получение индекса элемента
    /// @return Индекс элемента
    int32_t GetIndex() const;

    /// @brief Получение адреса тэга
    /// @return Адрес тэга
    std::string GetAddress() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

protected:
    TagKind::Type kind_;                ///< Тип тэга
    const std::string descriptionEx_;   ///< Расширенное описание (для настроечных параметров)
    const std::string range_;           ///< Диапазон (для настроечных параметров)
    bool isBasicParam_;                 ///< Является ли параметр базовым
    uint32_t updateRate_;               ///< Частота обновления параметра
    int32_t index_;                     ///< Номер элемента в массиве (для массивов). -1 - не задан
    std::string address_;               ///< Адрес тэга

}; // class DataTagDef

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_DATA_TAG_DEF_H
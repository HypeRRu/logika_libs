/// @file Определение типа тэга Logika6
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA6_DATA_TAG_DEF6_H
#define LOGIKA_METERS_LOGIKA6_DATA_TAG_DEF6_H

#include <logika/meters/data_tag_def.h>

namespace logika
{

namespace meters
{

/// @brief Тип элемента Logika6
enum class Tag6NodeType
{
    Tag,        ///< Тэг (параметр)
    Array,      ///< Массив тэгов
    Structure   ///< Структура
}; // enum class Tag6NodeType


/// @brief Параметры для создания тэга Logika6
struct DataTagDef6Settings: public DataTagDefSettings
{
public:
    Tag6NodeType nodeType = Tag6NodeType::Tag;  ///< Тип элемента
    uint32_t count = 0;                         ///< Количество элементов в массиве (для массивов)
    int32_t index = 0;                          ///< Номер элемента в массиве (для массивов), -1 - не задан

}; // struct DataTagDef6Settings


/// @brief Тэг с данными Logika6
class DataTagDef6: public DataTagDef
{
public:
    /// @brief Конструктор тэга с данными Logika6
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга с данными Logika6
    DataTagDef6( const ChannelDef& cdef, const DataTagDef6Settings& settings );

    /// @brief Получение типа элемента
    /// @return Тип элемента
    Tag6NodeType GetNodeType() const;

    /// @brief Получение количества элементов в массиве
    /// @return Количество элементов в массиве
    uint32_t GetCount() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

protected:
    Tag6NodeType nodeType_; ///< Тип элемента
    uint32_t count_;        ///< Количество элементов в массиве (для массивов)

}; // class DataTagDef6

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA6_DATA_TAG_DEF6_H

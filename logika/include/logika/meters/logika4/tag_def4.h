/// @file Определение типа тэга Logika4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_TAG_DEF4_H
#define LOGIKA_METERS_LOGIKA4_TAG_DEF4_H

#include <logika/meters/tag_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания тэга Logika4
struct DataTagDef4Settings: public DataTagDefSettings
{
public:
    std::string units = ""; ///< Единицы измерения

}; // struct DataTagDef4Settings


/// @brief Базовый тэг с данными Logika4
class DataTagDef4: public DataTagDef
{
public:
    /// @brief Конструктор тэга Logika4
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга Logika4
    DataTagDef4( const ChannelDef& cdef, const DataTagDef4Settings& settings );

    /// @brief Получение единиц измерения
    /// @return Единицы измерения
    std::string GetUnits() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

protected:
    std::string units_; ///< Единицы измерения

}; // class DataTagDef4

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_TAG_DEF4_H

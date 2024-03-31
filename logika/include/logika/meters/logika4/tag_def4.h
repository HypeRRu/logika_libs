/// @file Определение типа тэга Logika4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_TAG_DEF4_H
#define LOGIKA_METERS_LOGIKA4_TAG_DEF4_H

#include <logika/meters/tag_def.h>
#include <logika/meters/data_tag_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания тэга Logika4
struct TagDef4Settings: public DataTagDefSettings
{
public:
    LocString units = L""; ///< Единицы измерения

}; // struct TagDef4Settings


/// @brief Базовый тэг Logika4
class TagDef4: public DataTagDef
{
public:
    /// @brief Конструктор тэга Logika4
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга Logika4
    TagDef4( const ChannelDef& cdef, const TagDef4Settings& settings );

    /// @brief Получение единиц измерения
    /// @return Единицы измерения
    LocString GetUnits() const;

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

protected:
    LocString units_; ///< Единицы измерения

}; // class TagDef4

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_TAG_DEF4_H

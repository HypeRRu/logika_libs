/// @file Определение типа тэга Logika4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_TAG_DEF4_H
#define LOGIKA_METERS_LOGIKA4_TAG_DEF4_H

#include <logika/meters/defs.h>
#include <logika/meters/tag_def.h>
#include <logika/meters/data_tag_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания тэга Logika4
struct LOGIKA_METERS_EXPORT TagDef4Settings: public DataTagDefSettings
{
public:
    LocString units = L""; ///< Единицы измерения

}; // struct TagDef4Settings


/// @brief Базовый тэг Logika4
class LOGIKA_METERS_EXPORT TagDef4: public DataTagDef
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


/// @brief Параметры расчетного тэга
struct LOGIKA_METERS_EXPORT CalcFieldDefSettings: public TagDefSettings
{
public:
    int32_t channelNo       = -1;   ///< Номер канала
    LocString insertAfter   = L"";  ///< Имя тэга, после которого должен располагатся данный
    LocString expression    = L"";  ///< Выражение
    LocString eu            = L"";  ///< Единицы измерения

}; // CalcFieldDefSettings


/// @brief Расчетный тэг
class LOGIKA_METERS_EXPORT CalcFieldDef: public TagDef
{
public:
    /// @brief Конструктор расчетного тэга
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры расчетного тэга
    CalcFieldDef( const ChannelDef& cdef, const CalcFieldDefSettings& settings );

    /// @brief Получение номера канала
    /// @return Номер канала
    int32_t GetChannelNo() const;

    /// @brief Получение имени тэга, после которого располагается данный
    /// @return Имя тэга
    LocString GetInsertAfter() const;

    /// @brief Получение выражения
    /// @return Выражение
    LocString GetExpression() const;

    /// @brief Получение единиц измерения
    /// @return Единицы измерения
    LocString GetEu() const;

protected:
    int32_t channelNo_;     ///< Номер канала
    LocString insertAfter_; ///< Имя тэга, после которого должен располагатся данный
    LocString expression_;  ///< Выражение
    LocString eu_;          ///< Единицы измерения

}; // CalcFieldDef

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_TAG_DEF4_H

/// @file Определение интерфейса прибора
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_IMETER_H
#define LOGIKA_METERS_IMETER_H

#include <logika/common/iserializable.h>
#include <logika/common/types.h>

#include <logika/meters/types.h>

/// @cond
#include <string>
#include <memory>
/// @endcond

namespace logika
{

namespace meters
{

class DataTagDefVault;

/// @brief Интерфейс прибора
class IMeter: public ISerializable
{
public:
    virtual ~IMeter() = default;

    /// @brief Получение типа измерения
    /// @return Тип измерения
    virtual MeasureKind GetMeasureKind() const = 0;

    /// @brief Получение названия прибора
    /// @return Название прибора
    virtual LocString GetCaption() const = 0;

    /// @brief Получение описания прибора
    /// @return Описание прибора
    virtual LocString GetDescription() const = 0;

    /// @brief Получение максимального количества каналов
    /// @return Максимальное количество каналов
    virtual uint32_t GetMaxChannels() const = 0;

    /// @brief Получение максимального количества групп
    /// @return Максимальное количество групп
    virtual uint32_t GetMaxGroups() const = 0;

    /// @brief Получение типа канала
    /// @return Тип канала
    virtual ChannelKind GetChannelKind() const = 0;

    /// @brief Получение краткого названия производителя
    /// @return Краткое название производителя
    virtual LocString GetVendorId() const = 0;

    /// @brief Получение полного названия производителя
    /// @return Полное название производителя
    virtual LocString GetVendor() const = 0;

    /// @brief Получение типа протокола
    /// @return Тип протокола
    virtual BusProtocolType GetBusType() const = 0;

    /// @brief Получение префикса события
    /// @param[in] tv @todo ???
    /// @return Префикс события
    virtual LocString GetEventPrefix( uint32_t tv ) const = 0;

    /// @brief Получение хранилища описаний тэгов
    /// @return Хранилище описаний тэгов
    virtual const std::shared_ptr< DataTagDefVault > GetTagsVault() const = 0;

}; // class Meter

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_IMETER_H

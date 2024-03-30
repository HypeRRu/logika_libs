/// @file Определение базового класса прибора
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_METER_H
#define LOGIKA_METERS_METER_H

#include <logika/meters/imeter.h>
#include <logika/common/shared_constructible.hpp>

namespace logika
{

namespace meters
{

/// @brief Базовый класс прибора
class Meter: public IMeter, public SharedConstructible< Meter >
{
public:
    /// @brief Конструктор класса прибора
    /// @param[in] measureKind Тип измерения
    /// @param[in] caption Название прибора
    /// @param[in] description Описание прибора
    /// @param[in] maxChannels Максимальное количество каналов
    /// @param[in] maxGroups Максимальное количество групп
    /// @param[in] busType Тип протокола
    Meter(
          MeasureKind measureKind           = MeasureKind::Undefined
        , const std::string& caption        = ""
        , const LocString& description      = L""
        , uint32_t maxChannels              = 1
        , uint32_t maxGroups                = 1
        , BusProtocolType busType           = BusProtocolType::Undefined
    );

    bool operator ==( const Meter& other ) const;
    bool operator !=( const Meter& other ) const;

    /// @copydoc IMeter::GetMeasureKind()
    virtual MeasureKind GetMeasureKind() const override;

    /// @copydoc IMeter::GetCaption()
    virtual std::string GetCaption() const override;

    /// @copydoc IMeter::GetDescription()
    virtual LocString GetDescription() const override;

    /// @copydoc IMeter::GetMaxChannels()
    virtual uint32_t GetMaxChannels() const override;

    /// @copydoc IMeter::GetMaxGroups()
    virtual uint32_t GetMaxGroups() const override;

    /// @copydoc IMeter::GetChannelKind()
    virtual ChannelKind GetChannelKind() const override;

    /// @copydoc IMeter::GetVendorId()
    virtual LocString GetVendorId() const override;

    /// @copydoc IMeter::GetVendor()
    virtual LocString GetVendor() const override;

    /// @copydoc IMeter::GetBusType()
    virtual BusProtocolType GetBusType() const override;

    /// @copydoc IMeter::GetEventPrefix()
    virtual std::string GetEventPrefix( uint32_t tv ) const override;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

    /// @todo hash code?
    /// @todo get typed meters

protected:
    MeasureKind measureKind_;   ///< Тип измерения
    std::string caption_;       ///< Название прибора
    LocString description_;     ///< Описание прибора
    uint32_t maxChannels_;      ///< Максимальное количество каналов
    uint32_t maxGroups_;        ///< Максимальное количество групп
    bool supportedByProlog4_;   ///< Имеется ли поддержка prolog4
    bool outdated_;             ///< Является ли прибор устаревшим
    ChannelKind channelKind_;   ///< Тип канала
    BusProtocolType busType_;   ///< Тип протокола
    /// @todo mutex?
    /// @todo tags?


}; // class Meter

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_METER_H

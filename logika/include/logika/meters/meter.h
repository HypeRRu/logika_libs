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
        , const LocString& caption          = L""
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
    virtual LocString GetCaption() const override;

    /// @copydoc IMeter::GetDescription()
    virtual LocString GetDescription() const override;

    /// @copydoc IMeter::GetMaxChannels()
    virtual uint32_t GetMaxChannels() const override;

    /// @copydoc IMeter::GetMaxGroups()
    virtual uint32_t GetMaxGroups() const override;

    /// @copydoc IMeter::GetChannelKind()
    virtual ChannelKind GetChannelKind( int32_t cStart, uint32_t cCount, const LocString& cName ) const override;

    /// @copydoc IMeter::GetVendorId()
    virtual LocString GetVendorId() const override;

    /// @copydoc IMeter::GetVendor()
    virtual LocString GetVendor() const override;

    /// @copydoc IMeter::GetBusType()
    virtual BusProtocolType GetBusType() const override;

    /// @copydoc IMeter::GetEventPrefix()
    virtual LocString GetEventPrefix( uint32_t tv ) const override;

    /// @copydoc IMeter::GetTagsVault()
    virtual const std::shared_ptr< DataTagDefVault > GetTagsVault() const override;

    /// @copydoc IMeter::GetDisplayFormat()
    virtual LocString GetDisplayFormat( std::shared_ptr< TagDef > def ) const override;

    /// @copydoc IMeter::IdentMatch()
    virtual bool IdentMatch( ByteType id0, ByteType id1, ByteType version ) const override;

    /// @copydoc IMeter::GetNtFromTag()
    virtual bool GetNtFromTag( const LocString& value, ByteType& out ) const override;

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

    /// @todo hash code?
    /// @todo get typed meters
    /// @todo well known tags
    /// @todo lookup common tags

protected:
    MeasureKind measureKind_;                       ///< Тип измерения
    LocString caption_;                             ///< Название прибора
    LocString description_;                         ///< Описание прибора
    uint32_t maxChannels_;                          ///< Максимальное количество каналов
    uint32_t maxGroups_;                            ///< Максимальное количество групп
    bool supportedByProlog4_;                       ///< Имеется ли поддержка prolog4
    bool outdated_;                                 ///< Является ли прибор устаревшим
    BusProtocolType busType_;                       ///< Тип протокола
    std::shared_ptr< DataTagDefVault > tagsVault_;  ///< Хранилище описаний тэгов
    uint16_t ident_;                                ///< Идентификатор прибора
    /// @todo mutex?

}; // class Meter

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_METER_H

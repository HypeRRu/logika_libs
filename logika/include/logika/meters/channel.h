/// @file Определение типа канала
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CHANNEL_H
#define LOGIKA_METERS_CHANNEL_H

#include <logika/meters/defs.h>
#include <logika/meters/types.h>

#include <logika/common/iserializable.h>

/// @cond
#include <string>
#include <memory>

#include <cstdint>
/// @endcond

namespace logika
{

namespace meters
{

class LOGIKA_METERS_EXPORT Meter;

/// @brief Структура описание канала
struct LOGIKA_METERS_EXPORT ChannelDef: public ISerializable
{
public:
    /// @brief Конструктор описания канала
    /// @param[in] m Прибор
    /// @param[in] p Префикс канала
    /// @param[in] s Начало
    /// @param[in] c Количество
    /// @param[in] d Описание канала
    ChannelDef( std::shared_ptr< Meter > m, const LocString& p
        , int32_t s , uint32_t c, const LocString& d );

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

public:
    const std::shared_ptr< Meter > meter;   ///< Прибор
    const ChannelKind kind;                 ///< Тип канала
    const LocString prefix;                 ///< Префикс канала
    const int32_t start;                    ///< Начало
    const uint32_t count;                   ///< Количество
    const LocString description;            ///< Описание канала

}; // struct ChannelDef


/// @brief Структура канала
struct LOGIKA_METERS_EXPORT Channel: public ChannelDef
{
public:
    /// @brief Конструктор канала
    /// @param[in] cdef Описание канала
    /// @param[in] channelNo Номер канала
    Channel( const ChannelDef& cdef, int32_t channelNo );

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

public:
    const int32_t no;       ///< Номер канала
    const LocString name;   ///< Имя канала

}; // struct Channel

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CHANNEL_H

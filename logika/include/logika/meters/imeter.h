/// @file Определение интерфейса прибора
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_IMETER_H
#define LOGIKA_METERS_IMETER_H

#include <logika/meters/types.h>

namespace logika
{

namespace meters
{

/// @brief Интерфейс прибора
/// @todo Написать интерфейс
class IMeter
{
public:
    virtual ~IMeter() = default;

    /// @brief Получение типа канала
    /// @return Тип канала
    virtual ChannelKind GetChannelKind() const = 0;

}; // class Meter

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_IMETER_H

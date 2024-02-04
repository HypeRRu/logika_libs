/// @file Определение базового класса прибора
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_METER_H
#define LOGIKA_METERS_METER_H

#include <logika/meters/imeter.h>

namespace logika
{

namespace meters
{

/// @brief Базовый класс прибора
class Meter: public IMeter
{
public:
    /// @copydoc IMeter::GetChannelKind()
    virtual ChannelKind GetChannelKind() const override;

}; // class Meter

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_METER_H

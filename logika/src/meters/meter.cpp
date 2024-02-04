/// @file Реализация базового класса прибора
/// @copyright HypeRRu 2024

#include <logika/meters/meter.h>

namespace logika
{

namespace meters
{

ChannelKind Meter::GetChannelKind() const
{
    /// @todo real kind
    return ChannelKind::Undefined;
} // GetChannelKind

} // namespace meters

} // namespace logika

/// @file Реализация базового класса прибора
/// @copyright HypeRRu 2024

#include <logika/meters/meter.h>

namespace logika
{

namespace meters
{


Meter::Meter(
      MeasureKind measureKind
    , const std::string& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : measureKind_{ measureKind }
    , caption_{ caption }
    , description_{ description }
    , maxChannels_{ maxChannels }
    , maxGroups_{ maxGroups }
    , supportedByProlog4_{ false }
    , outdated_{ false }
    , channelKind_{ ChannelKind::Undefined }
    , busType_{ busType }
{
    /// @todo Определение ChannelKind
} // Meter


bool Meter::operator ==( const Meter& other ) const
{
    return      measureKind_    == other.measureKind_
            &&  caption_        == other.caption_
            &&  channelKind_    == other.channelKind_
            &&  busType_        == other.busType_;
} // operator ==


bool Meter::operator !=( const Meter& other ) const
{
    return !(*this == other);
} // operator !=


MeasureKind Meter::GetMeasureKind() const
{
    return measureKind_;
} // GetMeasureKind


std::string Meter::GetCaption() const
{
    return caption_;
} // GetCaption


LocString Meter::GetDescription() const
{
    return description_;
} // GetDescription


uint32_t Meter::GetMaxChannels() const
{
    return maxChannels_;
} // GetMaxChannels


uint32_t Meter::GetMaxGroups() const
{
    return maxGroups_;
} // GetMaxGroups


ChannelKind Meter::GetChannelKind() const
{
    return channelKind_;
} // GetChannelKind


LocString Meter::GetVendorId() const
{
    return L"ЛОГИКА";
} // GetVendorId


LocString Meter::GetVendor() const
{
    return L"ЗАО НПФ ЛОГИКА";
} // GetVendor


BusProtocolType Meter::GetBusType() const
{
    return busType_;
} // GetBusType


std::string Meter::GetEventPrefix( uint32_t tv ) const
{
    (void) tv;
    return ""; /// @todo
} // GetEventPrefix


std::string Meter::ToString() const
{
    return caption_;
} // ToString

} // namespace meters

} // namespace logika

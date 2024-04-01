/// @file Реализация базового класса прибора
/// @copyright HypeRRu 2024

#include <logika/meters/meter.h>

#include <logika/meters/data_tag_def.h>

namespace logika
{

namespace meters
{

Meter::Meter(
      MeasureKind measureKind
    , const LocString& caption
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
    , tagsVault_{}
{
    /// @todo Определение ChannelKind
    /// @todo Создание tagsVault_
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


LocString Meter::GetCaption() const
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


LocString Meter::GetEventPrefix( uint32_t tv ) const
{
    (void) tv;
    return L""; /// @todo Реализовать
} // GetEventPrefix


const std::shared_ptr< DataTagDefVault > Meter::GetTagsVault() const
{
    return tagsVault_;
} // GetTagsVault


LocString Meter::ToString() const
{
    return caption_;
} // ToString

} // namespace meters

} // namespace logika

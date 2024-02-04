/// @file Реализация типа тэга Logika4L
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/tag_def4l.h>

namespace logika
{

namespace meters
{

DataTagDef4L::DataTagDef4L( const ChannelDef& cdef, const DataTagDef4LSettings& settings )
    : DataTagDef4( cdef, settings )
    , internalType_{ settings.intType }
    , inRam_{ settings.inRam }
    , address_{ settings.address }
    , channelOffset_{ settings.cOffset }
    , addonAddress_{ settings.addonAddress }
    , addonChannelOffset_{ settings.cAddonOffset }
{} // DataTagDef4L


BinaryType4L::Type DataTagDef4L::GetInternalType() const
{
    return internalType_;
} // GetInternalType


bool DataTagDef4L::IsInRam() const
{
    return inRam_;
} // IsInRam


MeterAddressType DataTagDef4L::GetAddress() const
{
    return address_;
} // GetAddress


MeterAddressType DataTagDef4L::GetChannelOffset() const
{
    return channelOffset_;
} // GetChannelOffset


MeterAddressType DataTagDef4L::GetAddonAddress() const
{
    return addonAddress_;
} // GetAddonAddress


MeterAddressType DataTagDef4L::GetAddonChannelOffset() const
{
    return addonChannelOffset_;
} // GetAddonChannelOffset

} // namespace meters

} // namespace logika

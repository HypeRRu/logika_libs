/// @file Реализация типа тэга Logika4L
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/tag_def4l.h>

namespace logika
{

namespace meters
{

TagDef4L::TagDef4L( const ChannelDef& cdef, const TagDef4LSettings& settings )
    : TagDef4( cdef, settings )
    , internalType_{ settings.intType }
    , inRam_{ settings.inRam }
    , address_{ settings.address }
    , channelOffset_{ settings.cOffset }
    , addonAddress_{ settings.addonAddress }
    , addonChannelOffset_{ settings.cAddonOffset }
{} // TagDef4L


BinaryType4L::Type TagDef4L::GetInternalType() const
{
    return internalType_;
} // GetInternalType


bool TagDef4L::IsInRam() const
{
    return inRam_;
} // IsInRam


MeterAddressType TagDef4L::GetAddress() const
{
    return address_;
} // GetAddress


MeterAddressType TagDef4L::GetChannelOffset() const
{
    return channelOffset_;
} // GetChannelOffset


MeterAddressType TagDef4L::GetAddonAddress() const
{
    return addonAddress_;
} // GetAddonAddress


MeterAddressType TagDef4L::GetAddonChannelOffset() const
{
    return addonChannelOffset_;
} // GetAddonChannelOffset

} // namespace meters

} // namespace logika

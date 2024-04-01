/// @file Реализация конвертеров типов для каналов
/// @copyright HypeRRu 2024

#include <logika/meters/converters/channel_converter.h>

#include <logika/common/types.h>
#include <logika/common/misc.h>

/// @cond
#include <unordered_map>
/// @endcond

namespace logika
{

namespace meters
{

namespace converters
{

ChannelConverter::ConvertedType ChannelConverter::Convert(
    const ChannelConverter::FromType& from, ChannelConverter::StorageType meterStorage )
{
    return std::make_shared< BaseConvertedType >(
          ConvertDevice( ToLocString( from.device() ), meterStorage )
        , ToLocString( from.key() )
        , from.start()
        , from.count()
        , ToLocString( from.description() )
    );
} // Convert( const FromType&, StorageType )


ChannelConverter::ConvertedTypeList ChannelConverter::Convert(
    const ChannelConverter::FromTypeList& fromList, ChannelConverter::StorageType meterStorage )
{
    ConvertedTypeList converted;
    if ( !fromList.list_size() )
    {
        return converted;
    }
    for ( auto from: fromList.list() )
    {
        converted.push_back( Convert( from, meterStorage ) );
    }
    return converted;
} // Convert( const FromTypeList&, StorageType )


std::shared_ptr< Meter > ChannelConverter::ConvertDevice(
    const LocString& devName, ChannelConverter::StorageType meterStorage )
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    std::shared_ptr< Meter > device;
    meterStorage->GetItem( devName, device );
    return device;
} // ConvertDevice

} // namespace converters

} // namespace meters

} // namespace logika

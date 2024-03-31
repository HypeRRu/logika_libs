/// @file Реализация конвертеров типов для каналов
/// @copyright HypeRRu 2024

#include <logika/meters/converters/channel_converter.h>

#include <logika/common/types.h>
#include <logika/common/misc.h>

#include <logika/storage/storage.hpp>

/// @cond
#include <unordered_map>
/// @endcond

namespace logika
{

namespace meters
{

namespace converters
{

ChannelConverter::ConvertedType ChannelConverter::Convert( const ChannelConverter::FromType& from )
{
    return std::make_shared< BaseConvertedType >(
          ConvertDevice( ToLocString( from.device() ) )
        , ToLocString( from.key() )
        , from.start()
        , from.count()
        , ToLocString( from.description() )
    );
} // Convert( const ChannelConverter::FromType& from )


ChannelConverter::ConvertedTypeList ChannelConverter::Convert( const ChannelConverter::FromTypeList& fromList )
{
    ConvertedTypeList converted;
    if ( !fromList.list_size() )
    {
        return converted;
    }
    for ( auto from: fromList.list() )
    {
        converted.push_back( ChannelConverter::Convert( from ) );
    }
    return converted;
} // Convert( const ChannelConverter::FromTypeList& fromList )


std::shared_ptr< Meter > ChannelConverter::ConvertDevice( const LocString& devName );
{
    logika::storage::StorageKeeper sKeeper = logika::storage::StorageKeeper::Instance();
    auto mStorage = sKeeper.GetStorage< logika::LocString, logika::meters::Meter >();
    
    std::shared_ptr< Meter > device;
    mStorage.Get( devName, device );
    return device;
} // ConvertBusType

} // namespace converters

} // namespace meters

} // namespace logika

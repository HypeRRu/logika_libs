#include "converters/channels.h"

#include <string>
#include <fstream>

#include "third_party/csv.hpp"

#include <logika/resources/channels.pb.h>

namespace logika_migration
{

namespace Channels
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::ChannelList channelList;

    for ( auto& row: reader )
    {
        logika::resources::Channel* channel = channelList.add_list();
        if ( !channel )
        {
            return false;
        }

        channel->set_device( row[ "Device" ].get<>() );
        channel->set_key( row[ "Key" ].get<>() );
        channel->set_description( row[ "Description" ].get<>() );
        channel->set_start( row[ "Start" ].get< uint32_t >() );
        channel->set_count( row[ "Count" ].get< uint32_t >() );
    }

    std::ofstream outFile{ outFileName };
    return channelList.SerializeToOstream( &outFile );
} // Migrate

} // namespace Channels

} // namespace logika_migration

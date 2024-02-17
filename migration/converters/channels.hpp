#pragma once

#ifndef CHANNELS_H
#define CHANNELS_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "channels.pb.h"

namespace logika_migration
{

namespace Channels
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::ChannelList channelList;

    for ( auto& row: reader )
    {
        logika::Channel* channel = channelList.add_list();
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
} // migrate

} // namespace Channels

} // namespace logika_migration

#endif // CHANNELS_H

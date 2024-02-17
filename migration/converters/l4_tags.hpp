#pragma once

#ifndef L4_TAGS_H
#define L4_TAGS_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "l4_tags.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace L4Tags
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::L4TagList tags;

    for ( auto& row: reader )
    {
        logika::L4Tag* tag = tags.add_list();
        if ( !tag )
        {
            return false;
        }

        tag->set_device( row[ "Device" ].get<>() );
        tag->set_channel( row[ "Channel" ].get<>() );
        tag->set_ordinal( row[ "Ordinal" ].get< uint32_t >() );
        tag->set_kind(
            EnumsStringToPb::TagKindFromString( row[ "Kind" ].get<>() )
        );
        tag->set_basic( row[ "Basic" ].get< bool >() );
        tag->set_datatype(
            EnumsStringToPb::DataTypeFromString( row[ "DataType" ].get<>() )
        );
        if ( !row[ "dbType" ].is_null() )
        {
            tag->set_dbtype( row[ "dbType" ].get<>() );
        }
        tag->set_updaterate( row[ "UpdateRate" ].get< uint32_t >() );
        tag->set_name( row[ "Name" ].get<>() );
        if ( !row[ "DisplayFormat" ].is_null() )
        {
            tag->set_displayformat( row[ "DisplayFormat" ].get<>() );
        }
        tag->set_description( row[ "Description" ].get<>() );
        if ( !row[ "VarT" ].is_null() )
        {
            tag->set_vartype(
                EnumsStringToPb::VarTypeFromString( row[ "VarT" ].get<>() )
            );
        }
        if ( !row[ "Units" ].is_null() )
        {
            tag->set_units( row[ "Units" ].get<>() );
        }
        tag->set_internaltype(
            EnumsStringToPb::InternalTypeFromString( row[ "InternalType" ].get<>() )
        );
        tag->set_inram( row[ "InRAM" ].get< bool >() );
        if ( !row[ "Address" ].is_null() )
        {
            tag->set_address( row[ "Address" ].get< uint32_t >() );
        }
        if ( !row[ "ChannelOffset" ].is_null() )
        {
            tag->set_channeloffset( row[ "ChannelOffset" ].get< uint32_t >() );
        }
        if ( !row[ "Addon" ].is_null() )
        {
            tag->set_addon( row[ "Addon" ].get< uint32_t >() );
        }
        if ( !row[ "AddonOffset" ].is_null() )
        {
            tag->set_addonoffset( row[ "AddonOffset" ].get< uint32_t >() );
        }
        if ( !row[ "Range" ].is_null() )
        {
            tag->set_range( row[ "Range" ].get<>() );
        }
        if ( !row[ "DescriptionEx" ].is_null() )
        {
            tag->set_descriptionex( row[ "DescriptionEx" ].get<>() );
        }
    }

    std::ofstream outFile{ outFileName };
    return tags.SerializeToOstream( &outFile );
} // migrate

} // namespace L4Tags

} // namespace logika_migration

#endif // L4_TAGS_H

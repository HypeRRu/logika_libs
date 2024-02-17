#pragma once

#ifndef M4_TAGS_H
#define M4_TAGS_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "m4_tags.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace M4Tags
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::M4TagList tags;

    for ( auto& row: reader )
    {
        logika::M4Tag* tag = tags.add_list();
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

} // namespace M4Tags

} // namespace logika_migration

#endif // M4_TAGS_H

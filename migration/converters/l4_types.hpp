#pragma once

#ifndef L4_TYPES_H
#define L4_TYPES_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "l4_types.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace L4Types
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::L4Types types;

    for ( auto& row: reader )
    {
        logika::L4Type* type = types.add_types();
        if ( !type )
        {
            return false;
        }

        type->set_type(
            EnumsStringToPb::InternalTypeFromString( row[ "Type" ].get<>() )
        );
        type->set_description( row[ "Description" ].get<>() );

        if ( !row[ "Comment" ].is_null() )
        {
            type->set_comment( row[ "Comment" ].get<>() );
        }
    }

    std::ofstream outFile{ outFileName };
    return types.SerializeToOstream( &outFile );
} // migrate

} // namespace L4Types

} // namespace logika_migration

#endif // L4_TYPES_H

#pragma once

#ifndef ARCHIVE_TYPES_H
#define ARCHIVE_TYPES_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "archive_types.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace ArchiveTypes
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::ArchiveTypes types;

    for ( auto& row: reader )
    {
        logika::ArchiveType* type = types.add_types();
        if ( !type )
        {
            return false;
        }

        type->set_type(
            EnumsStringToPb::ArchiveTypeFromString( row[ "Type" ].get<>() )
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

} // namespace ArchiveTypes

} // namespace logika_migration

#endif // ARCHIVE_TYPES_H

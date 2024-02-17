#pragma once

#ifndef X6_TAG_TYPES_H
#define X6_TAG_TYPES_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "x6_tag_types.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace X6TagTypes
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::X6TagTypeList typesList;

    for ( auto& row: reader )
    {
        typesList.add_list(
            EnumsStringToPb::X6TagTypeFromString( row[ "type" ].get<>() )
        );
    }

    std::ofstream outFile{ outFileName };
    return typesList.SerializeToOstream( &outFile );
} // migrate

} // namespace X6TagTypes

} // namespace logika_migration

#endif // X6_TAG_TYPES_H

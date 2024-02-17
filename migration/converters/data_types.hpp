#pragma once

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "data_types.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace DataTypes
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::DataTypes types;

    for ( auto& row: reader )
    {
        logika::DataType* type = types.add_types();
        if ( !type )
        {
            return false;
        }

        type->set_name(
            EnumsStringToPb::DataTypeFromString( row[ "Name" ].get<>() )
        );
        type->set_description( row[ "Description" ].get<>() );
    }

    std::ofstream outFile{ outFileName };
    return types.SerializeToOstream( &outFile );
} // migrate

} // namespace DataTypes

} // namespace logika_migration

#endif // DATA_TYPES_H

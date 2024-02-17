#pragma once

#ifndef BUSES_H
#define BUSES_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "buses.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace Buses
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::BusList busList;

    for ( auto& row: reader )
    {
        logika::Bus* bus = busList.add_list();
        if ( !bus )
        {
            return false;
        }

        bus->set_key(
            EnumsStringToPb::BusTypeFromString( row[ "Key" ].get<>() )
        );
        bus->set_description( row[ "Description" ].get<>() );
    }

    std::ofstream outFile{ outFileName };
    return busList.SerializeToOstream( &outFile );
} // migrate

} // namespace Buses

} // namespace logika_migration

#endif // BUSES_H

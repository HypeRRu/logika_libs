#include "converters/buses.h"

#include <fstream>

#include "utils/enums_string_to_pb.h"
#include "third_party/csv.hpp"

#include <logika/resources/buses.pb.h>

namespace logika_migration
{

namespace Buses
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::BusList busList;

    for ( auto& row: reader )
    {
        logika::resources::Bus* bus = busList.add_list();
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
} // Migrate

} // namespace Buses

} // namespace logika_migration


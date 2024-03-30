#include "converters/data_types.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/data_types.pb.h>

namespace logika_migration
{

namespace DataTypes
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::DataTypes types;

    for ( auto& row: reader )
    {
        logika::resources::DataType* type = types.add_list();
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
} // Migrate

} // namespace DataTypes

} // namespace logika_migration

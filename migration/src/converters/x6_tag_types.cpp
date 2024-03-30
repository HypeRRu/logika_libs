#include "converters/x6_tag_types.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/x6_tag_types.pb.h>

namespace logika_migration
{

namespace X6TagTypes
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::X6TagTypeList typesList;

    for ( auto& row: reader )
    {
        typesList.add_list(
            EnumsStringToPb::X6TagTypeFromString( row[ "type" ].get<>() )
        );
    }

    std::ofstream outFile{ outFileName };
    return typesList.SerializeToOstream( &outFile );
} // Migrate

} // namespace X6TagTypes

} // namespace logika_migration

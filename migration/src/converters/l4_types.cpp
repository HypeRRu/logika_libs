#include "converters/l4_types.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/l4_types.pb.h>

namespace logika_migration
{

namespace L4Types
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::L4Types types;

    for ( auto& row: reader )
    {
        logika::resources::L4Type* type = types.add_list();
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
} // Migrate

} // namespace L4Types

} // namespace logika_migration

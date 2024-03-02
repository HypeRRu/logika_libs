#include "converters/archive_types.h"

#include <fstream>

#include "utils/enums_string_to_pb.h"
#include "third_party/csv.hpp"

#include <logika/resources/archive_types.pb.h>

namespace logika_migration
{

namespace ArchiveTypes
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::ArchiveTypes types;

    for ( auto& row: reader )
    {
        logika::resources::ArchiveType* type = types.add_list();
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
} // Migrate

} // namespace ArchiveTypes

} // namespace logika_migration

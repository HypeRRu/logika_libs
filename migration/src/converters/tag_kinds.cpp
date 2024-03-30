#include "converters/tag_kinds.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/tag_kinds.pb.h>

namespace logika_migration
{

namespace TagKinds
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::TagKindList tags;

    for ( auto& row: reader )
    {
        logika::resources::TagKind* tag = tags.add_list();
        if ( !tag )
        {
            return false;
        }

        tag->set_kind(
            EnumsStringToPb::TagKindFromString( row[ "Kind" ].get<>() )
        );
        tag->set_description( row[ "Description" ].get<>() );
    }

    std::ofstream outFile{ outFileName };
    return tags.SerializeToOstream( &outFile );
} // Migrate

} // namespace TagKinds

} // namespace logika_migration

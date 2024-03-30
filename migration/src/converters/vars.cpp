#include "converters/vars.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/vars.pb.h>

namespace logika_migration
{

namespace Vars
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::VarList vars;

    for ( auto& row: reader )
    {
        logika::resources::Var* var = vars.add_list();
        if ( !var )
        {
            return false;
        }

        var->set_kind(
            EnumsStringToPb::VarTypeFromString( row[ "varT" ].get<>() )
        );
        var->set_description( row[ "desc" ].get<>() );
    }

    std::ofstream outFile{ outFileName };
    return vars.SerializeToOstream( &outFile );
} // Migrate

} // namespace Vars

} // namespace logika_migration

#pragma once

#ifndef VARS_H
#define VARS_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "vars.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace Vars
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::VarList vars;

    for ( auto& row: reader )
    {
        logika::Var* var = vars.add_list();
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
} // migrate

} // namespace Vars

} // namespace logika_migration

#endif // VARS_H

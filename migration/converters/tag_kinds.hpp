#pragma once

#ifndef TAG_KINDS_H
#define TAG_KINDS_H

#include <string>
#include <fstream>

#include "csv.hpp"
#include "tag_kinds.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace TagKinds
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::TagKindList tags;

    for ( auto& row: reader )
    {
        logika::TagKind* tag = tags.add_list();
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
} // migrate

} // namespace TagKinds

} // namespace logika_migration

#endif // TAG_KINDS_H

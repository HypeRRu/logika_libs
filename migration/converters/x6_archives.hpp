#pragma once

#ifndef X6_ARCHIVES_H
#define X6_ARCHIVES_H

#include <string>
#include <fstream>
#include <sstream>

#include "csv.hpp"
#include "x6_archives.pb.h"

#include "../utils/enums_string_to_pb.hpp"

namespace logika_migration
{

namespace X6Archives
{

bool migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::X6ArchiveList archiveList;

    for ( auto& row: reader )
    {
        logika::X6Archive* archive = archiveList.add_list();
        if ( !archive )
        {
            return false;
        }

        archive->set_device( row[ "Device" ].get<>() );
        archive->set_archivetype(
            EnumsStringToPb::ArchiveTypeFromString( row[ "ArchiveType" ].get<>() )
        );

        std::istringstream ordinals{ row[ "Ordinal" ].get<>() };
        uint32_t ordinal;
        while ( ordinals >> ordinal )
        {
            archive->add_ordinals( ordinal );
        }
        archive->set_recordtype(
            EnumsStringToPb::DataTypeFromString( row[ "RecordType" ].get<>() )
        );
        archive->set_name( row[ "Name" ].get<>() );
        archive->set_description( row[ "Description" ].get<>() );
        archive->set_capacity( row[ "Capacity" ].get< uint32_t >() );
    }

    std::ofstream outFile{ outFileName };
    return archiveList.SerializeToOstream( &outFile );
} // migrate

} // namespace X6Archives

} // namespace logika_migration

#endif // X6_ARCHIVES_H

#include "converters/m4_archives.h"

#include <fstream>

#include "utils/enums_string_to_pb.h"
#include "third_party/csv.hpp"

#include <logika/resources/m4_archives.pb.h>

namespace logika_migration
{

namespace M4Archives
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::M4ArchiveList archiveList;

    for ( auto& row: reader )
    {
        logika::M4Archive* archive = archiveList.add_list();
        if ( !archive )
        {
            return false;
        }

        archive->set_device( row[ "Device" ].get<>() );
        archive->set_archivetype(
            EnumsStringToPb::ArchiveTypeFromString( row[ "ArchiveType" ].get<>() )
        );
        archive->set_channel( row[ "Channel" ].get<>() );
        archive->set_recordtype(
            EnumsStringToPb::DataTypeFromString( row[ "RecordType" ].get<>() )
        );
        archive->set_name( row[ "Name" ].get<>() );
        archive->set_description( row[ "Description" ].get<>() );
        archive->set_capacity( row[ "Capacity" ].get< uint32_t >() );
    }

    std::ofstream outFile{ outFileName };
    return archiveList.SerializeToOstream( &outFile );
} // Migrate

} // namespace M4Archives

} // namespace logika_migration

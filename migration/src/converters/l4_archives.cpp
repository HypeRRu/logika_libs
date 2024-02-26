#include "converters/l4_archives.h"

#include <fstream>

#include "utils/enums_string_to_pb.h"
#include "third_party/csv.hpp"

#include <logika/resources/l4_archives.pb.h>

namespace logika_migration
{

namespace L4Archives
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::L4ArchiveList archiveList;

    for ( auto& row: reader )
    {
        logika::L4Archive* archive = archiveList.add_list();
        if ( !archive )
        {
            return false;
        }

        archive->set_device( row[ "Device" ].get<>() );
        archive->set_archivetype(
            EnumsStringToPb::ArchiveTypeFromString( row[ "ArchiveType" ].get<>() )
        );
        archive->set_channel( row[ "Channel" ].get<>() );
        archive->set_name( row[ "Name" ].get<>() );
        archive->set_description( row[ "Description" ].get<>() );
        archive->set_recordtype(
            EnumsStringToPb::DataTypeFromString( row[ "RecordType" ].get<>() )
        );
        archive->set_recordsize( row[ "RecordSize" ].get< uint32_t >() );
        archive->set_count( row[ "Count" ].get< uint32_t >() );
        
        archive->set_indexaddr( row[ "Index1" ].get< uint32_t >() );
        if ( !row[ "Headers1" ].is_null() )
        {
            archive->set_headersaddr( row[ "Headers1" ].get< uint32_t >() );
        }
        archive->set_recordsaddr( row[ "Records1" ].get< uint32_t >() );
        
        if ( !row[ "Index2" ].is_null() )
        {
            archive->set_indexaddr2( row[ "Index2" ].get< uint32_t >() );
        }
        if ( !row[ "Headers2" ].is_null() )
        {
            archive->set_headersaddr2( row[ "Headers2" ].get< uint32_t >() );
        }
        if ( !row[ "Records2" ].is_null() )
        {
            archive->set_recordsaddr2( row[ "Records2" ].get< uint32_t >() );
        }
    }

    std::ofstream outFile{ outFileName };
    return archiveList.SerializeToOstream( &outFile );
} // Migrate

} // namespace L4Archives

} // namespace logika_migration

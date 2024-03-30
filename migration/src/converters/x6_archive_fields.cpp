#include "converters/x6_archive_fields.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/x6_archive_fields.pb.h>

namespace logika_migration
{

namespace X6ArchiveFields
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::X6ArchiveFieldList fieldList;

    for ( auto& row: reader )
    {
        logika::resources::X6ArchiveField* field = fieldList.add_list();
        if ( !field )
        {
            return false;
        }

        field->set_device( row[ "Device" ].get<>() );
        field->set_archivetype(
            EnumsStringToPb::ArchiveTypeFromString( row[ "ArchiveType" ].get<>() )
        );
        field->set_channel( row[ "Channel" ].get<>() );
        field->set_ordinal( row[ "Ordinal" ].get< uint32_t >() );
        field->set_datatype(
            EnumsStringToPb::DataTypeFromString( row[ "DataType" ].get<>() )
        );
        if ( !row[ "dbType" ].is_null() )
        {
            field->set_dbtype( row[ "dbType" ].get<>() );
        }
        field->set_name( row[ "Name" ].get<>() );
        field->set_description( row[ "Description" ].get<>() );

        if ( !row[ "VarT" ].is_null() )
        {
            field->set_vartype(
                EnumsStringToPb::VarTypeFromString( row[ "VarT" ].get<>() )
            );
        }
        if ( !row[ "StartIndex" ].is_null() )
        {
            field->set_startindex( row[ "StartIndex" ].get< uint32_t >() );
        }
        if ( !row[ "Count" ].is_null() )
        {
            field->set_count( row[ "Count" ].get< uint32_t >() );
        }
        if ( !row[ "Depth" ].is_null() )
        {
            field->set_depth( row[ "Depth" ].get< uint32_t >() );
        }
        if ( !row[ "_comment" ].is_null() )
        {
            field->set_comment( row[ "_comment" ].get<>() );
        }
    }

    std::ofstream outFile{ outFileName };
    return fieldList.SerializeToOstream( &outFile );
} // Migrate

} // namespace X6ArchiveFields

} // namespace logika_migration

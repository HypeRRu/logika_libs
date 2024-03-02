#include "converters/m4_archive_fields.h"

#include <fstream>

#include "utils/enums_string_to_pb.h"
#include "third_party/csv.hpp"

#include <logika/resources/m4_archive_fields.pb.h>

namespace logika_migration
{

namespace M4ArchiveFields
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::M4ArchiveFieldList fieldList;

    for ( auto& row: reader )
    {
        logika::resources::M4ArchiveField* field = fieldList.add_list();
        if ( !field )
        {
            return false;
        }

        field->set_device( row[ "Device" ].get<>() );
        field->set_archivetype(
            EnumsStringToPb::ArchiveTypeFromString( row[ "ArchiveType" ].get<>() )
        );
        field->set_index( row[ "Index" ].get< uint32_t >() );
        field->set_name( row[ "Name" ].get<>() );
        field->set_datatype(
            EnumsStringToPb::DataTypeFromString( row[ "DataType" ].get<>() )
        );
        field->set_description( row[ "Description" ].get<>() );

        if ( !row[ "VarT" ].is_null() )
        {
            field->set_vartype(
                EnumsStringToPb::VarTypeFromString( row[ "VarT" ].get<>() )
            );
        }
        if ( !row[ "Units" ].is_null() )
        {
            field->set_units( row[ "Units" ].get<>() );
        }
        if ( !row[ "dbType" ].is_null() )
        {
            field->set_dbtype( row[ "dbType" ].get<>() );
        }
        if ( !row[ "DisplayFormat" ].is_null() )
        {
            field->set_displayformat( row[ "DisplayFormat" ].get<>() );
        }
    }

    std::ofstream outFile{ outFileName };
    return fieldList.SerializeToOstream( &outFile );
} // Migrate

} // namespace M4ArchiveFields

} // namespace logika_migration

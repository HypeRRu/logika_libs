#include "converters/l4_archive_fields.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/l4_archive_fields.pb.h>

namespace logika_migration
{

namespace L4ArchiveFields
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::L4ArchiveFieldList fieldList;

    for ( auto& row: reader )
    {
        logika::resources::L4ArchiveField* field = fieldList.add_list();
        if ( !field )
        {
            return false;
        }

        field->set_device( row[ "Device" ].get<>() );
        field->set_archivetype(
            EnumsStringToPb::ArchiveTypeFromString( row[ "ArchiveType" ].get<>() )
        );
        field->set_fieldoffset( row[ "FieldOffset" ].get< uint32_t >() );
        field->set_name( row[ "Name" ].get<>() );
        field->set_internaltype(
            EnumsStringToPb::InternalTypeFromString( row[ "InternalType" ].get<>() )
        );
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
    }

    std::ofstream outFile{ outFileName };
    return fieldList.SerializeToOstream( &outFile );
} // Migrate

} // namespace L4ArchiveFields

} // namespace logika_migration

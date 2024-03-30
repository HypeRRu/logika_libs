#include "converters/x6_tags.h"

/// @cond
#include <fstream>

#include "third_party/csv.hpp"
/// @endcond

#include "utils/enums_string_to_pb.h"

#include <logika/resources/x6_tags.pb.h>

namespace logika_migration
{

namespace X6Tags
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::resources::X6TagList tags;

    for ( auto& row: reader )
    {
        logika::resources::X6Tag* tag = tags.add_list();
        if ( !tag )
        {
            return false;
        }

        tag->set_device( row[ "Device" ].get<>() );
        tag->set_channel( row[ "Channel" ].get<>() );
        tag->set_ordinal( row[ "Ordinal" ].get< uint32_t >() );
        if ( !row[ "Index" ].is_null() )
        {
            tag->set_index( row[ "Index" ].get< uint32_t >() );
        }
        tag->set_type(
            EnumsStringToPb::X6TagTypeFromString( row[ "Type" ].get<>() )
        );
        if ( !row[ "Kind" ].is_null() )
        {
            tag->set_kind(
                EnumsStringToPb::TagKindFromString( row[ "Kind" ].get<>() )
            );
        }
        tag->set_basic( row[ "Basic" ].get< bool >() );
        if ( !row[ "DataType" ].is_null() )
        {
            tag->set_datatype(
                EnumsStringToPb::DataTypeFromString( row[ "DataType" ].get<>() )
            );
        }
        tag->set_updaterate( row[ "UpdateRate" ].get< uint32_t >() );
        if ( !row[ "Name" ].is_null() )
        {
            tag->set_name( row[ "Name" ].get<>() );
        }
        tag->set_description( row[ "Description" ].get<>() );
        if ( !row[ "VarT" ].is_null() )
        {
            tag->set_vartype(
                EnumsStringToPb::VarTypeFromString( row[ "VarT" ].get<>() )
            );
        }
        if ( !row[ "Count" ].is_null() )
        {
            tag->set_count( row[ "Count" ].get< uint32_t >() );
        }
        if ( !row[ "Range" ].is_null() )
        {
            tag->set_range( row[ "Range" ].get<>() );
        }
        if ( !row[ "DescriptionEx" ].is_null() )
        {
            tag->set_descriptionex( row[ "DescriptionEx" ].get<>() );
        }
    }

    std::ofstream outFile{ outFileName };
    return tags.SerializeToOstream( &outFile );
} // Migrate

} // namespace X6Tags

} // namespace logika_migration

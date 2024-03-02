#include "converters/devices.h"

#include <fstream>

#include "third_party/csv.hpp"

#include <logika/resources/devices.pb.h>

namespace logika_migration
{

namespace Devices
{

bool Migrate( const std::string& inFileName, const std::string& outFileName )
{
    csv::CSVReader reader( inFileName );
    logika::DeviceList deviceList;

    for ( auto& row: reader )
    {
        logika::Device* device = deviceList.add_list();
        if ( !device )
        {
            return false;
        }

        device->set_key( row[ "Key" ].get<>() );
        device->set_bus( row[ "Bus" ].get<>() );
        device->set_m4( !row[ "M4" ].is_null() && row[ "M4" ].get< bool >() );
        device->set_description( row[ "Description" ].get<>() );
        device->set_media( row[ "Media" ].get<>() );

        if ( !row[ "Pipes" ].is_null() )
        {
            device->set_pipes( row[ "Pipes" ].get< uint32_t >() );
        }
        if ( !row[ "Consumers" ].is_null() )
        {
            device->set_consumers( row[ "Consumers" ].get< uint32_t >() );
        }
        if ( !row[ "auxNo" ].is_null() )
        {
            device->set_auxno( row[ "auxNo" ].get< uint32_t >() );
        }
    }

    std::ofstream outFile{ outFileName };
    return deviceList.SerializeToOstream( &outFile );
} // Migrate

} // namespace Devices

} // namespace logika_migration
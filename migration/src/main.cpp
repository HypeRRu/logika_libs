#include <iostream>

#include "converters/archive_types.h"
#include "converters/buses.h"
#include "converters/channels.h"
#include "converters/data_types.h"
#include "converters/devices.h"
#include "converters/l4_archive_fields.h"
#include "converters/l4_archives.h"
#include "converters/l4_types.h"
#include "converters/l4_tags.h"
#include "converters/m4_archive_fields.h"
#include "converters/m4_archives.h"
#include "converters/m4_tags.h"
#include "converters/tag_kinds.h"
#include "converters/vars.h"
#include "converters/x6_archive_fields.h"
#include "converters/x6_archives.h"
#include "converters/x6_tags.h"
#include "converters/x6_tag_types.h"

int main( int argc, char** argv )
{
    constexpr char archive_types_path[] = "ArchiveTypes";
    constexpr char buses_path[] = "Buses";
    constexpr char channels_path[] = "Channels";
    constexpr char data_types_path[] = "DataTypes";
    constexpr char devices_path[] = "Devices";
    constexpr char l4_archive_fields_path[] = "L4ArchiveFields";
    constexpr char l4_archives_path[] = "L4Archives";
    constexpr char l4_types_path[] = "L4Types";
    constexpr char l4_tags_path[] = "L4Tags";
    constexpr char m4_archive_fields_path[] = "M4ArchiveFields";
    constexpr char m4_archives_path[] = "M4Archives";
    constexpr char m4_tags_path[] = "M4Tags";
    constexpr char vars_path[] = "Vars";
    constexpr char x6_archive_fields_path[] = "X6ArchiveFields";
    constexpr char x6_archives_path[] = "X6Archives";
    constexpr char x6_tags_path[] = "X6Tags";
    constexpr char x6_tag_types_path[] = "X6tagType";

    if ( argc != 3 )
    {
        std::cerr << "usage: " << argv[ 0 ] << " <csv_dir> <out_dir>\n";
        return 1;
    }

    logika_migration::ArchiveTypes::Migrate(
          std::string{ argv[ 1 ] } + "/" + archive_types_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + archive_types_path + ".dat"
    );
    logika_migration::Buses::Migrate(
          std::string{ argv[ 1 ] } + "/" + buses_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + buses_path + ".dat"
    );
    logika_migration::Channels::Migrate(
          std::string{ argv[ 1 ] } + "/" + channels_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + channels_path + ".dat"
    );
    logika_migration::DataTypes::Migrate(
          std::string{ argv[ 1 ] } + "/" + data_types_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + data_types_path + ".dat"
    );
    logika_migration::Devices::Migrate(
          std::string{ argv[ 1 ] } + "/" + devices_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + devices_path + ".dat"
    );
    logika_migration::L4ArchiveFields::Migrate(
          std::string{ argv[ 1 ] } + "/" + l4_archive_fields_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + l4_archive_fields_path + ".dat"
    );
    logika_migration::L4Archives::Migrate(
          std::string{ argv[ 1 ] } + "/" + l4_archives_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + l4_archives_path + ".dat"
    );
    logika_migration::L4Types::Migrate(
          std::string{ argv[ 1 ] } + "/" + l4_types_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + l4_types_path + ".dat"
    );
    logika_migration::L4Tags::Migrate(
          std::string{ argv[ 1 ] } + "/" + l4_tags_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + l4_tags_path + ".dat"
    );
    logika_migration::M4ArchiveFields::Migrate(
          std::string{ argv[ 1 ] } + "/" + m4_archive_fields_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + m4_archive_fields_path + ".dat"
    );
    logika_migration::M4Archives::Migrate(
          std::string{ argv[ 1 ] } + "/" + m4_archives_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + m4_archives_path + ".dat"
    );
    logika_migration::M4Tags::Migrate(
          std::string{ argv[ 1 ] } + "/" + m4_tags_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + m4_tags_path + ".dat"
    );
    logika_migration::Vars::Migrate(
          std::string{ argv[ 1 ] } + "/" + vars_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + vars_path + ".dat"
    );
    logika_migration::X6ArchiveFields::Migrate(
          std::string{ argv[ 1 ] } + "/" + x6_archive_fields_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + x6_archive_fields_path + ".dat"
    );
    logika_migration::X6Archives::Migrate(
          std::string{ argv[ 1 ] } + "/" + x6_archives_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + x6_archives_path + ".dat"
    );
    logika_migration::X6Tags::Migrate(
          std::string{ argv[ 1 ] } + "/" + x6_tags_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + x6_tags_path + ".dat"
    );
    logika_migration::X6TagTypes::Migrate(
          std::string{ argv[ 1 ] } + "/" + x6_tag_types_path + ".csv"
        , std::string{ argv[ 2 ] } + "/" + x6_tag_types_path + ".dat"
    );

    return 0;
}

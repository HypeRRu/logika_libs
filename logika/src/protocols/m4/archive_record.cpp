/// @file Реализация структуры записи архива в протоколе M4
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/archive_record.h>

#include <logika/common/misc.h>

/// @cond
#include <sstream>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{

ArchiveRecord::ArchiveRecord()
    : timeSinceStart{ 0 }
    , fullTime{ 0 }
    , values{}
{} // ArchiveRecord()


ArchiveRecord::ArchiveRecord( TimeType sinceStart, TimeType full,
    const std::vector< std::shared_ptr< logika::Any > >& vals )
    : timeSinceStart{ sinceStart }
    , fullTime{ full }
    , values{ vals }
{} // ArchiveRecord( TimeType, TimeType, const std::vector< std::shared_ptr< logika::Any > >& )


LocString ArchiveRecord::ToString() const
{
    LocStringStream stream;
    stream << ToLocString( GetTimeString( fullTime ) );

    if ( !values.empty() )
    {
        stream << LOCALIZED( "{" );
        stream << ( values[ 0 ] ? LOCALIZED( "not null" ) : LOCALIZED( "null" ) );
    }
    for ( size_t i = 1; i < values.size(); ++i )
    {
        stream << LOCALIZED( ", " ) << ( values[ i ] ? LOCALIZED( "not null" ) : LOCALIZED( "null" ) );
    }
    if ( !values.empty() )
    {
        stream << LOCALIZED( "}" );
    }

    return stream.str();
} // ToString

} // namespace M4

} // namespace protocols

} // namespace logika

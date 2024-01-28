/// @file Реализация структуры записи архива в протоколе M4
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/archive_record.h>

#include <logika/common/misc.h>

#include <sstream>

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


ArchiveRecord::ArchiveRecord( TimeType sinceStart, TimeType full, const std::vector< ISerializable* >& vals )
    : timeSinceStart{ sinceStart }
    , fullTime{ full }
    , values{ vals }
{} // ArchiveRecord( TimeType sinceStart, TimeType full, const std::vector< ISerializable* >& vals )


std::string ArchiveRecord::ToString() const
{
    std::stringstream stream;
    stream << GetTimeString( fullTime ) << "";

    if ( !values.empty() )
    {
        stream << "{";
        stream << ( values[ 0 ] ? values[ 0 ]->ToString() : "null" );
    }
    for ( size_t i = 1; i < values.size(); ++i )
    {
        stream << ", " << ( values[ i ] ? values[ i ]->ToString() : "null" );
    }
    if ( !values.empty() )
    {
        stream << "}";
    }

    return stream.str();
} // ToString

} // namespace M4

} // namespace protocols

} // namespace logika

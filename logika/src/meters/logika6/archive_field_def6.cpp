/// @file Реализация описания поля архива Logika6
/// @copyright HypeRRu 2024

#include <logika/meters/logika6/archive_field_def6.h>

/// @cond
#include <sstream>
#include <iomanip>
/// @endcond

namespace logika
{

namespace meters
{

ArchiveFieldDef6::ArchiveFieldDef6( const ChannelDef& cdef, const ArchiveFieldDef6Settings& settings )
    : ArchiveFieldDef( cdef, settings )
    , nameSuffixed_{ settings.name }
    , address_{ "" }
{
    std::stringstream ss;
    ss << std::setw( 3 ) << std::setfill( '0' ) << ordinal_;
    address_ = ss.str();
    const size_t leftPos = name_.find_first_of( '(' );
    if ( std::string::npos != leftPos
        && name_.length() - 1 == name_.find_last_of( leftPos, ')' ) )
    {
        name_ = name_.substr( 0, leftPos );
    }
} // ArchiveFieldDef6


std::string ArchiveFieldDef6::GetNameSuffixed() const
{
    return nameSuffixed_;
} // GetNameSuffixed


std::string ArchiveFieldDef6::GetAddress() const
{
    return address_;
} // GetAddress

} // namespace meters

} // namespace logika

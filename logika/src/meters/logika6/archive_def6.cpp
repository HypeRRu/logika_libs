/// @file Реализация типа описания архива Logika6
/// @copyright HypeRRu 2024

#include <logika/meters/logika6/archive_def6.h>

/// @cond
#include <stdexcept>
#include <sstream>
#include <iomanip>
/// @endcond

namespace logika
{

namespace meters
{

/// Описание архива Logika6

ArchiveDef6::ArchiveDef6( const ChannelDef& cdef, const ArchiveDef6Settings& settings )
    : ArchiveDef( cdef, settings )
    , address_{ L"" }
{
    LocStringStream ss;
    ss << std::setw( 3 ) << std::setfill( L'0' ) << ordinal_;
    address_ = ss.str();
} // ArchiveDef6


LocString ArchiveDef6::GetAddress() const
{
    return address_;
} // GetAddress


/// Описание многочастного архива Logika6

MultipartArchiveDef6::MultipartArchiveDef6( const ChannelDef& cdef, const MultipartArchiveDef6Settings& settings )
    : ArchiveDef( cdef, settings )
    , ordinals_{ settings.ordinals }
{} // MultipartArchiveDef6


int MultipartArchiveDef6::GetOrdinal() const
{
    throw std::runtime_error{ "'ordinal' is not available for multipart archives" };
} // GetOrdinal


const std::vector< int > MultipartArchiveDef6::GetOrdinals() const
{
    return ordinals_;
} // GetOrdinals

} // namespace meters

} // namespace logika

/// @file Реализация описания поля архива Logika4
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/archive_field_def4.h>

#include <logika/common/misc.h>

namespace logika
{

namespace meters
{

ArchiveFieldDef4::ArchiveFieldDef4( const ChannelDef& cdef
    , const ArchiveFieldDef4Settings& settings )
    : ArchiveFieldDef( cdef, settings )
    , archive_{ settings.archive }
    , units_{ settings.units }
{
    key_        = name_;
    ordinal_    = -1;
    /// @bug А в чем смысл?
    address_    = ToLocString( std::to_string( ordinal_ ) );
} // ArchiveFieldDef4


void ArchiveFieldDef4::SetArchive( std::shared_ptr< ArchiveDef > archive )
{
    archive_ = archive;
} // SetArchive


std::shared_ptr< ArchiveDef > ArchiveFieldDef4::GetArchive() const
{
    return archive_;
} // GetArchive


LocString ArchiveFieldDef4::GetUnits() const
{
    return units_;
} // GetUnits

} // namespace meters

} // namespace logika

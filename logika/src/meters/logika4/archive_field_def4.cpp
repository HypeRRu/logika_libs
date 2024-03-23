/// @file Реализация описания поля архива Logika4
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/archive_field_def4.h>

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
} // ArchiveFieldDef4


std::shared_ptr< ArchiveDef > ArchiveFieldDef4::GetArchive() const
{
    return archive_;
} // GetArchive


std::string ArchiveFieldDef4::GetUnits() const
{
    return units_;
} // GetUnits

} // namespace meters

} // namespace logika

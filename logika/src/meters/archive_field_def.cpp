/// @file Реализация описания поля архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive_field_def.h>

namespace logika
{

namespace meters
{

/// @todo Аккуратно с ArchiveType::All().at( settings.archiveTypeName )
/// Возможно разыменование указателя на освобожденную память
ArchiveFieldDef::ArchiveFieldDef( const ChannelDef& cdef
    , const ArchiveFieldDefSettings& settings, const ArchiveType& archiveType )
    : TagDef( cdef, settings )
    , archiveType_{ ArchiveType::All().at( settings.archiveTypeName ) }
{} // ArchiveFieldDef

} // namespace meters

} // namespace logika

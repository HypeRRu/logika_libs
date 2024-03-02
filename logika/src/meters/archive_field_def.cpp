/// @file Реализация описания поля архива
/// @copyright HypeRRu 2024

#include <logika/meters/archive_field_def.h>

#include <logika/storage/storage.hpp>

namespace logika
{

namespace meters
{

ArchiveFieldDef::ArchiveFieldDef( const ChannelDef& cdef
    , const ArchiveFieldDefSettings& settings )
    : TagDef( cdef, settings )
    , archiveType_{}
{
    auto storage = storage::StorageKeeper::Instance().GetStorage< std::string, ArchiveType >();
    storage && storage->GetItem( settings.archiveTypeName, archiveType_ );
} // ArchiveFieldDef


std::shared_ptr< ArchiveType > ArchiveFieldDef::GetArchiveType() const
{
    return archiveType_;
} // ArchiveFieldDef

} // namespace meters

} // namespace logika

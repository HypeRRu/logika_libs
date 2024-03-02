/// @file Определение описания поля архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ARCHIVE_FIELD_DEF_H
#define LOGIKA_METERS_ARCHIVE_FIELD_DEF_H

#include <logika/meters/tag_def.h>
#include <logika/meters/types.h>

#include <logika/common/iserializable.h>

#include <string>

namespace logika
{

namespace meters
{

struct ArchiveFieldDefSettings: public TagDefSettings
{
public:
    std::string archiveTypeName;    ///< Название типа архива

}; // struct ArchiveFieldDefSettings


class ArchiveFieldDef: public TagDef
{
public:
    ArchiveFieldDef( const ChannelDef& cdef, const ArchiveFieldDefSettings& settings );

    std::shared_ptr< ArchiveType > GetArchiveType() const;

protected:
    std::shared_ptr< ArchiveType > archiveType_;

}; // class ArchiveDef

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ARCHIVE_FIELD_DEF_H

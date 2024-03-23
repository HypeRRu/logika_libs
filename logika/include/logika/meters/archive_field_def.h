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

/// @brief Параметры описания поля архива
struct ArchiveFieldDefSettings: public TagDefSettings
{
public:
    std::shared_ptr< ArchiveType > archiveType = nullptr;   ///< Название типа архива

}; // struct ArchiveFieldDefSettings


/// @brief Описание поля архива
class ArchiveFieldDef: public TagDef, public ISerializable
{
public:
    /// @brief Конструктор описания поля архива
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания поля архива
    ArchiveFieldDef( const ChannelDef& cdef, const ArchiveFieldDefSettings& settings );

    /// @brief Получение типа архива
    /// @return Тип архива
    std::shared_ptr< ArchiveType > GetArchiveType() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

protected:
    std::shared_ptr< ArchiveType > archiveType_;    ///< Тип архива

}; // class ArchiveFieldDef

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ARCHIVE_FIELD_DEF_H

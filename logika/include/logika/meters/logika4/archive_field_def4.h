/// @file Определение описания поля архива Logika4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_ARCHIVE_FIELD_DEF4_H
#define LOGIKA_METERS_LOGIKA4_ARCHIVE_FIELD_DEF4_H

#include <logika/meters/logika4/defs.h>
#include <logika/meters/archive_field_def.h>
#include <logika/meters/archive_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры описания поля архива Logika4
struct LOGIKA_METERS_4_EXPORT ArchiveFieldDef4Settings: public ArchiveFieldDefSettings
{
public:
    std::shared_ptr< ArchiveDef > archive = nullptr;    ///< Описание архива
    LocString units = LOCALIZED( "" );                  ///< Единицы измерения

}; // struct ArchiveFieldDef4Settings


/// @brief Описание поля архива Logika4
class LOGIKA_METERS_4_EXPORT ArchiveFieldDef4: public ArchiveFieldDef
{
public:
    /// @brief Конструктор описания поля архива Logika4
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания поля архива Logika4
    ArchiveFieldDef4( const ChannelDef& cdef, const ArchiveFieldDef4Settings& settings );

    /// @brief Изменение описания архива
    /// @return Описание архива
    void SetArchive( std::shared_ptr< ArchiveDef > archive );

    /// @brief Получение описания архива
    /// @return Описание архива
    std::shared_ptr< ArchiveDef > GetArchive() const;

    /// @brief Получение единиц измерения
    /// @return Единицы измерения
    LocString GetUnits() const;

protected:
    std::shared_ptr< ArchiveDef > archive_; ///< Описание архива
    LocString units_;                       ///< Единицы измерения

}; // class ArchiveFieldDef4

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_ARCHIVE_FIELD_DEF4_H

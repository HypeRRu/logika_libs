/// @file Определение описания поля архива Logika6
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA6_ARCHIVE_FIELD_DEF6_H
#define LOGIKA_METERS_LOGIKA6_ARCHIVE_FIELD_DEF6_H

#include <logika/meters/defs.h>
#include <logika/meters/archive_field_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры описания поля архива Logika6
struct LOGIKA_METERS_EXPORT ArchiveFieldDef6Settings: public ArchiveFieldDefSettings
{
public:
    
}; // struct ArchiveFieldDef6Settings


/// @brief Описание поля архива Logika6
class LOGIKA_METERS_EXPORT ArchiveFieldDef6: public ArchiveFieldDef
{
public:
    /// @brief Конструктор описания поля архива Logika6
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания поля архива Logika6
    ArchiveFieldDef6( const ChannelDef& cdef, const ArchiveFieldDef6Settings& settings );

    /// @brief Получение имени поля архива
    /// @return Имя поля архива
    LocString GetNameSuffixed() const;

protected:
    LocString address_;         ///< Адрес поля архива
    LocString nameSuffixed_;    ///< Имя поля архива

}; // class ArchiveFieldDef6

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA6_ARCHIVE_FIELD_DEF6_H

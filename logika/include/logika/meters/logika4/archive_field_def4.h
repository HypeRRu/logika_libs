/// @file Определение описания поля архива Logika4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_ARCHIVE_FIELD_DEF4_H
#define LOGIKA_METERS_LOGIKA4_ARCHIVE_FIELD_DEF4_H

#include <logika/meters/archive_field_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры описания поля архива Logika4
struct ArchiveFieldDef4Settings: public ArchiveFieldDefSettings
{
public:
    
}; // struct ArchiveFieldDef4Settings


class ArchiveFieldDef4: public ArchiveFieldDef
{
public:
    /// @brief Конструктор описания поля архива Logika4
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания поля архива Logika4
    ArchiveFieldDef4( const ChannelDef& cdef, const ArchiveFieldDef4Settings& settings );

    /// @brief Получение имени поля архива
    /// @return Имя поля архива
    std::string GetNameSuffixed() const;

    /// @brief Получение адреса поля
    /// @return Адрес поля архива
    std::string GetAddress() const;

protected:
    std::string address_;       ///< Адрес поля архива
    std::string nameSuffixed_;  ///< Имя поля архива

}; // class ArchiveFieldDef4

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA6_ARCHIVE_FIELD_DEF4_H

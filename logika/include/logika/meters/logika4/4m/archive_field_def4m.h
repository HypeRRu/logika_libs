/// @file Определение описания поля архива Logika4M
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_ARCHIVE_FIELD_DEF4M_H
#define LOGIKA_METERS_LOGIKA4_4M_ARCHIVE_FIELD_DEF4M_H

#include <logika/meters/logika4/4m/defs.h>
#include <logika/meters/logika4/archive_field_def4.h>

#include <logika/common/shared_constructible.hpp>

namespace logika
{

namespace meters
{

/// @brief Параметры описания поля архива Logika4L
struct LOGIKA_METERS_4M_EXPORT ArchiveFieldDef4MSettings: public ArchiveFieldDef4Settings
{
public:
    uint32_t fieldIndex = 0;    ///< Индекс поля

}; // struct ArchiveFieldDef4MSettings


/// @brief Описание поля архива Logika4M
class LOGIKA_METERS_4M_EXPORT ArchiveFieldDef4M: public ArchiveFieldDef4, public SharedConstructible< ArchiveFieldDef4M >
{
public:
    /// @brief Конструктор описания поля архива Logika4M
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания поля архива Logika4M
    ArchiveFieldDef4M( const ChannelDef& cdef, const ArchiveFieldDef4MSettings& settings );

    /// @brief Получение индекса поля
    /// @return Индекс поля
    uint32_t GetFieldIndex() const;

protected:
    uint32_t fieldIndex_;   ///< Индекс поля

}; // class ArchiveFieldDef4M

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_ARCHIVE_FIELD_DEF4M_H

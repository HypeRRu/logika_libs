/// @file Определение типа описания архива Logika4M
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_ARCHIVE_DEF4M_H
#define LOGIKA_METERS_LOGIKA4_4M_ARCHIVE_DEF4M_H

#include <logika/meters/archive_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания описания архива Logika6
struct ArchiveDef4MSettings: public ArchiveDefSettings
{
public:
}; // struct ArchiveDef6Settings


/// @brief Описание архива Logika4M
class ArchiveDef4M: public ArchiveDef
{
public:
    /// @brief Конструктор описания архива Logika4M
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания архива Logika4M
    ArchiveDef4M( const ChannelDef& cdef, const ArchiveDef4MSettings& settings );

}; // class ArchiveDef4M

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_ARCHIVE_DEF4M_H
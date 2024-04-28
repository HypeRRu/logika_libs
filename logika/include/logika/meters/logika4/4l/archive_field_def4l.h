/// @file Определение описания поля архива Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_ARCHIVE_FIELD_DEF4L_H
#define LOGIKA_METERS_LOGIKA4_4L_ARCHIVE_FIELD_DEF4L_H

#include <logika/meters/logika4/4l/defs.h>
#include <logika/meters/logika4/archive_field_def4.h>
#include <logika/meters/logika4/4l/types.h>

#include <logika/common/shared_constructible.hpp>

namespace logika
{

namespace meters
{

/// @brief Параметры описания поля архива Logika4L
struct LOGIKA_METERS_4L_EXPORT ArchiveFieldDef4LSettings: public ArchiveFieldDef4Settings
{
public:
    BinaryType4L::Type binType   = BinaryType4L::Undefined; ///< Бинарный тип поля
    MeterAddressType fieldOffset = 0x0;                     ///< Смещение поля

}; // struct ArchiveFieldDef4LSettings


/// @brief Описание поля архива Logika4L
class LOGIKA_METERS_4L_EXPORT ArchiveFieldDef4L: public ArchiveFieldDef4, public SharedConstructible< ArchiveFieldDef4L >
{
public:
    /// @brief Конструктор описания поля архива Logika4L
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания поля архива Logika4L
    ArchiveFieldDef4L( const ChannelDef& cdef, const ArchiveFieldDef4LSettings& settings );

    /// @brief Получение бинарного типа поля
    /// @return Бинарный тип поля
    BinaryType4L::Type GetInternalType() const;

    /// @brief Получение смещения поля
    /// @return Смещение поля
    MeterAddressType GetFieldOffset() const;

protected:
    BinaryType4L::Type internalType_;   ///< Бинарный тип поля
    MeterAddressType fieldOffset_;      ///< Смещение поля

}; // class ArchiveFieldDef4L

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_ARCHIVE_FIELD_DEF4L_H

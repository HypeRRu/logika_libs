/// @file Определение типа описания архива Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_ARCHIVE_DEF4L_H
#define LOGIKA_METERS_LOGIKA4_4L_ARCHIVE_DEF4L_H

#include <logika/meters/archive_def.h>

#include <logika/common/shared_constructible.hpp>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания описания архива Logika4L
struct ArchiveDef4LSettings: public ArchiveDefSettings
{
public:
    uint32_t recordSize             = 0;        ///< Размер записи
    MeterAddressType indexAddr      = 0x0;      ///< Адрес индекса
    MeterAddressType headerAddr     = 0x0;      ///< Адрес заголовка
    MeterAddressType recordsAddr    = 0x0;      ///< Адрес записей
    MeterAddressType indexAddr2     = 0x0;      ///< Адрес индекса (дополнительный)
    MeterAddressType headerAddr2    = 0x0;      ///< Адрес заголовка (дополнительный)
    MeterAddressType recordsAddr2   = 0x0;      ///< Адрес записей (дополнительный)
    bool isTiny42                   = false;    ///< Одноканальная версия

}; // struct ArchiveDef4LSettings


/// @brief Описание архива Logika4L
class ArchiveDef4L: public ArchiveDef, public SharedConstructible< ArchiveDef4L >
{
public:
    /// @brief Конструктор описания архива Logika4L
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания архива Logika4L
    ArchiveDef4L( const ChannelDef& cdef, const ArchiveDef4LSettings& settings );

    /// @brief Является ли архив одноканальным
    /// @return Одноканальный архив
    bool IsTiny() const;

    /// @brief Получение размер записи
    /// @return Размер записи
    uint32_t GetRecordSize() const;

    /// @brief Получение адреса индекса
    /// @return Адрес индекса
    MeterAddressType GetIndexAddr() const;

    /// @brief Получение адреса заголовка
    /// @return Адрес заголовка
    MeterAddressType GetHeaderAddr() const;

    /// @brief Получение адреса записей
    /// @return Адрес записей
    MeterAddressType GetRecordsAddr() const;

    /// @brief Получение дополнительного адреса индекса
    /// @return Дополнительный адрес индекса
    MeterAddressType GetIndexAddr2() const;

    /// @brief Получение дополнительного адреса заголовка
    /// @return Дополнительный адрес заголовка
    MeterAddressType GetHeaderAddr2() const;

    /// @brief Получение дополнительного адреса записей
    /// @return Дополнительный адрес записей
    MeterAddressType GetRecordsAddr2() const;

protected:
    bool isTiny42_;                 ///< Одноканальная версия 
    uint32_t recordSize_;           ///< Размер записи
    MeterAddressType indexAddr_;    ///< Адрес индекса
    MeterAddressType headerAddr_;   ///< Адрес заголовка
    MeterAddressType recordsAddr_;  ///< Адрес записей
    MeterAddressType indexAddr2_;   ///< Адрес индекса (дополнительный)
    MeterAddressType headerAddr2_;  ///< Адрес заголовка (дополнительный)
    MeterAddressType recordsAddr2_; ///< Адрес записей (дополнительный)

}; // class ArchiveDef4L

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_ARCHIVE_DEF4L_H

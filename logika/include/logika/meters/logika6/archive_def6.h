/// @file Определение типа описания архива Logika6
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA6_ARCHIVE_DEF6_H
#define LOGIKA_METERS_LOGIKA6_ARCHIVE_DEF6_H

#include <logika/meters/logika6/defs.h>
#include <logika/meters/archive_def.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания описания архива Logika6
struct LOGIKA_METERS_6_EXPORT ArchiveDef6Settings: public ArchiveDefSettings
{
public:

}; // struct ArchiveDef6Settings


/// @brief Описание архива Logika6
class LOGIKA_METERS_6_EXPORT ArchiveDef6: public ArchiveDef
{
public:
    /// @brief Конструктор описания архива Logika6
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания архива Logika6
    ArchiveDef6( const ChannelDef& cdef, const ArchiveDef6Settings& settings );

    /// @brief Получение адреса
    /// @return Адрес
    LocString GetAddress() const;

protected:
    LocString address_; ///< Адрес

}; // class ArchiveDef6


/// @brief Параметры для создания описания многочастного архива Logika6
struct LOGIKA_METERS_6_EXPORT MultipartArchiveDef6Settings: public ArchiveDefSettings
{
public:
    std::vector< int > ordinals; // Порядковые номера записей
}; // struct MultipartArchiveDef6Settings


/// @brief Описание многочастного архива Logika6
class LOGIKA_METERS_6_EXPORT MultipartArchiveDef6: public ArchiveDef
{
public:
    /// @brief Конструктор описания многочастного архива Logika6
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры описания многочастного архива Logika6
    MultipartArchiveDef6( const ChannelDef& cdef, const MultipartArchiveDef6Settings& settings );

    /// @copydoc ItemDefBase::GetOrdinal()
    virtual int GetOrdinal() const override;

    /// @brief Получение порядковых номеров записей
    /// @return Порядковые номера записей
    const std::vector< int > GetOrdinals() const;

protected:
    std::vector< int > ordinals_; // Порядковые номера записей

}; // class MultipartArchiveDef6

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA6_ARCHIVE_DEF6_H

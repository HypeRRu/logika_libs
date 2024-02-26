/// @file Определение архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ARCHIVE_H
#define LOGIKA_METERS_ARCHIVE_H

#include <logika/meters/types.h>

#include <logika/common/iserializable.h>

#include <string>

namespace logika
{

namespace meters
{

/// @brief Базовый класс архива
class Archive
{
public:
    /// @brief Конструктор архива
    /// @param[in] meter Прибор
    /// @param[in] archiveType Тип архива
    Archive( const Meter& meter, const ArchiveType& archiveType );

    /// @brief Получение прибора
    /// @return Прибор
    const Meter& GetMeter() const;

    /// @brief Получение типа архива
    /// @return Тип архива
    const ArchiveType& GetArchiveType() const;

public:
    static constexpr char FLD_EXTPROP_KEY = "AfInfo";   ///< @todo Имя поля для ресурсов

protected:
    const Meter& meter_;                ///< Прибор
    const ArchiveType& archiveType_;    ///< Тип архива
}; // class Archive


class IntervalArchive: public Archive
{
public:
/// @todo ArchiveFieldCollection
}; // class IntervalArchive

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ARCHIVE_H

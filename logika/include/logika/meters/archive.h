/// @file Определение архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ARCHIVE_H
#define LOGIKA_METERS_ARCHIVE_H

#include <logika/meters/defs.h>
#include <logika/meters/types.h>
#include <logika/meters/meter.h>

namespace logika
{

namespace meters
{

/// @brief Базовый класс архива
class LOGIKA_METERS_EXPORT Archive
{
public:
    /// @brief Конструктор архива
    /// @param[in] meter Прибор
    /// @param[in] archiveType Тип архива
    Archive( std::shared_ptr< Meter > meter, std::shared_ptr< ArchiveType > archiveType );

    virtual ~Archive() = default;

    /// @brief Получение прибора
    /// @return Прибор
    std::shared_ptr< Meter > GetMeter() const;

    /// @brief Получение типа архива
    /// @return Тип архива
    std::shared_ptr< ArchiveType > GetArchiveType() const;

protected:
    std::shared_ptr< Meter > meter_;                ///< Прибор
    std::shared_ptr< ArchiveType > archiveType_;    ///< Тип архива

}; // class Archive

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ARCHIVE_H

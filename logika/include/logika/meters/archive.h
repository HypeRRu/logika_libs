/// @file Определение архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ARCHIVE_H
#define LOGIKA_METERS_ARCHIVE_H

#include <logika/meters/types.h>
#include <logika/meters/meter.h>
#include <logika/meters/data_table.hpp>

#include <logika/common/iserializable.h>

#include <string>

namespace logika
{

namespace meters
{

/// @brief Базовый класс архива
/// @todo Может Meter передавать по shared_ptr?
/// @todo Разнести по файлам
class Archive
{
public:
    /// @brief Конструктор архива
    /// @param[in] meter Прибор
    /// @param[in] archiveType Тип архива
    Archive( const Meter& meter, std::shared_ptr< ArchiveType > archiveType );

    /// @brief Получение прибора
    /// @return Прибор
    const Meter& GetMeter() const;

    /// @brief Получение типа архива
    /// @return Тип архива
    std::shared_ptr< ArchiveType > GetArchiveType() const;

protected:
    const Meter& meter_;                            ///< Прибор
    std::shared_ptr< ArchiveType > archiveType_;    ///< Тип архива

}; // class Archive


class IntervalArchive: public Archive
{
public:
    IntervalArchive( const Meter& meter, std::shared_ptr< ArchiveType > archiveType );

    DataTable& GetDataTable();
    const DataTable& GetDataTable() const;

private:
    DataTable dataTable_;

}; // class IntervalArchive

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ARCHIVE_H

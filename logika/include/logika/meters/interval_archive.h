/// @file Определение интервального архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_INTERVAL_ARCHIVE_H
#define LOGIKA_METERS_INTERVAL_ARCHIVE_H

#include <logika/meters/defs.h>
#include <logika/meters/data_table.hpp>
#include <logika/meters/archive.h>

namespace logika
{

namespace meters
{

/// @brief Интервальный архив
class LOGIKA_METERS_EXPORT IntervalArchive: public Archive
{
public:
    /// @brief Конструктор интервального архива
    /// @param[in] meter Прибор
    /// @param[in] archiveType Тип архива
    IntervalArchive( std::shared_ptr< IMeter > meter, std::shared_ptr< ArchiveType > archiveType );
    
    /// @brief Конструктор интервального архива
    /// @param[in] meter Прибор
    /// @param[in] archiveType Тип архива
    /// @param[in] tableTemplate Схема таблицы данных
    IntervalArchive( std::shared_ptr< IMeter > meter
        , std::shared_ptr< ArchiveType > archiveType, std::shared_ptr< DataTable > tableTemplate );

    /// @brief Получение данных архива
    /// @return Данные архива
    std::shared_ptr< DataTable > GetDataTable();

    /// @brief Получение данных архива
    /// @return Данные архива
    const std::shared_ptr< DataTable > GetDataTable() const;

private:
    /// @brief Добавление в таблицу стандартных колонок
    /// @return Удалось ли добавить колонки
    /// @note Стандартные колонки: tm
    bool AddDefaultColumns();

private:
    std::shared_ptr< DataTable > dataTable_;    ///< Данные архива

}; // class IntervalArchive

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_INTERVAL_ARCHIVE_H

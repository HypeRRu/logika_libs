/// @file Определение таблицы данных
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TABLE_H
#define LOGIKA_METERS_DATA_TABLE_H

#include <logika/meters/types.h>
#include <logika/meters/archive_field.h>

#include <logika/common/any.hpp>

#include <unordered_map>
#include <vector>

namespace logika
{

namespace meters
{

/// @brief Таблица данных
class DataTable
{
public:
    using FieldType     = std::shared_ptr< ArchiveField >;
    using RecordType    = std::shared_ptr< std::vector< logika::Any > >;

    /// @brief Конструктор таблицы данных
    /// @param[in] fieldList Список полей таблицы
    DataTable( const std::vector< FieldType >& fieldList );

    DataTable( const DataTable& ) = delete;
    DataTable& operator= ( const DataTable& ) = delete;
    /// @todo Сделать move-able

    /// @brief Получение количества записей в таблице
    /// @return Количество записей в таблице
    size_t RecordCount() const;

    /// @brief Получение количества столбцов в таблице
    /// @return Количество столбцов в таблице
    size_t FieldCount() const;

    /// @brief Получение записи
    /// @param[in] index Номер записи
    /// @return Запись под указанным номером или nullptr, если не существует
    const RecordType GetRecord( size_t index ) const;

    /// @brief Извлечение последней записи
    /// @return Извлеченная запись или nullptr, если таблица пуста
    const RecordType PopRecord();

    /// @brief Удаление записи из таблицы
    /// @param[in] record Удаляемая запись
    /// @return Удалось ли удалить запись
    bool RemoveRecord( RecordType record );

    /// @brief Удаление записи из таблицы под заданным номером
    /// @param[in] index Номер добавляемой записи
    /// @return Удалось ли удалить запись
    bool RemoveRecord( size_t index );

    /// @brief Добавление записи в конец таблицы
    /// @param[in] record Добавляемая запись
    /// @return Удалось ли добавить запись
    bool PushRecord( RecordType record );

    /// @brief Добавление записи
    /// @param[in] record Добавляемая запись
    /// @param[in] index Индекс добавляемой записи
    /// @return Удалось ли добавить запись
    /// @note Если index > Count, то запись добавить нельзя
    bool InsertRecord( RecordType record, size_t index );

    /// @brief Получение столбца таблицы
    /// @tparam T тип данных в столбце
    /// @param[in] field Столбец
    /// @return Данные из столбца
    template < typename T >
    std::vector< T > GetColumn( FieldType field ) const;

    /// @brief Получение столбца таблицы
    /// @tparam T тип данных в столбце
    /// @param[in] fieldIndex Номер столбца
    /// @return Данные из столбца
    template < typename T >
    std::vector< T > GetColumn( size_t fieldIndex ) const;

private:
    /// @brief Проверка, что запись содержит верные поля
    /// @param[in] record Проверяемая запись
    /// @return Можно ли добавить запись в таблицу
    bool ValidateRecord( RecordType record );

private:
    std::vector< FieldType >    fieldList_; ///< Список полей
    std::vector< RecordType >   data_;      ///< Список записей

};

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_DATA_TABLE_H

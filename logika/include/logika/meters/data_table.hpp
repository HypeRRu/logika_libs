/// @file Определение таблицы данных
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TABLE_HPP
#define LOGIKA_METERS_DATA_TABLE_HPP

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
/// @todo Поиск по имени/caption столбца
class DataTable
{
public:
    using FieldType     = std::shared_ptr< ArchiveField >;
    using RecordType    = std::shared_ptr< std::vector< logika::Any > >;

    /// @brief Конструктор таблицы данных
    /// @param[in] fieldList Список полей таблицы
    DataTable( const std::vector< FieldType >& fieldList = std::vector< FieldType >{} );

    DataTable( const DataTable& ) = delete;
    DataTable& operator= ( const DataTable& ) = delete;
    /// @todo Сделать move-able

    /// @brief Получение количества записей в таблице
    /// @return Количество записей в таблице
    size_t RecordCount() const;

    /// @brief Получение количества столбцов в таблице
    /// @return Количество столбцов в таблице
    size_t FieldCount() const;

    /// @brief Получение списка столбцов таблицы
    /// @return Список столбцов таблицы
    const std::vector< FieldType >& GetFieldsList() const;

    /// @brief Получение списка записей в таблице
    /// @return Список записей в таблице
    const std::vector< RecordType >& GetRecordsList() const;

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

    /// @brief Добавление столбца таблицы
    /// @param[in] field Добавляемый столбец
    /// @return Удалось ли добавить столбец
    bool AddColumn( FieldType field );

    /// @brief Добавление столбца таблицы
    /// @tparam T тип данных в столбце
    /// @param[in] field Добавляемый столбец
    /// @param[in] value Значение по умолчанию
    /// @return Удалось ли добавить столбец
    template < typename T >
    bool AddColumn( FieldType field, const T& value );

    /// @brief Добавление столбца таблицы под определенным номером
    /// @param[in] field Добавляемый столбец
    /// @param[in] index Номер столбца
    /// @return Удалось ли добавить столбец
    bool InsertColumn( FieldType field, size_t index );

    /// @brief Добавление столбца таблицы под определенным номером
    /// @tparam T тип данных в столбце
    /// @param[in] field Добавляемый столбец
    /// @param[in] index Номер столбца
    /// @param[in] value Значение по умолчанию
    /// @return Удалось ли добавить столбец
    template < typename T >
    bool InsertColumn( FieldType field, size_t index, const T& value );

    /// @brief Удаление столбца таблицы
    /// @param[in] field Удаляемый столбец
    /// @return Удалось ли удалить столбец
    bool RemoveColumn( FieldType field );

    /// @brief Удаление столбца таблицы
    /// @param[in] fieldIndex Номер удаляемого столбца
    /// @return Удалось ли удалить столбец
    bool RemoveColumn( size_t fieldIndex );

private:
    /// @brief Проверка, что запись содержит верные поля
    /// @param[in] record Проверяемая запись
    /// @return Можно ли добавить запись в таблицу
    bool ValidateRecord( RecordType record );

    /// @brief Проверка, что можно добавить поле
    /// @param[in] field Проверяемое поле
    /// @return Можно ли добавить поле в таблицу
    bool ValidateField( FieldType field );

private:
    std::vector< FieldType >    fieldList_; ///< Список полей
    std::vector< RecordType >   data_;      ///< Список записей

};

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_DATA_TABLE_HPP

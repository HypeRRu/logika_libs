/// @file Определение сервисного архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_SERVICE_ARCHIVE_H
#define LOGIKA_METERS_SERVICE_ARCHIVE_H

#include <logika/meters/defs.h>
#include <logika/meters/archive.h>
#include <logika/common/iserializable.h>

namespace logika
{

namespace meters
{

/// @brief Запись сервисного архива
struct LOGIKA_METERS_EXPORT ServiceRecord: public ISerializable
{
public:
    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

public:
    TimeType timestamp = 0;                     ///< Метка времени записи
    LocString event = LOCALIZED( "" );          ///< Событие
    LocString description = LOCALIZED( "" );    ///< Описание события

}; // struct ServiceRecord


/// @brief Сервисный архив
class LOGIKA_METERS_EXPORT ServiceArchive: public Archive
{
public:
    using RecordType = std::shared_ptr< ServiceRecord >;

    /// @brief Конструктор архива
    /// @param[in] meter Прибор
    /// @param[in] archiveType Тип архива
    ServiceArchive( std::shared_ptr< IMeter > meter, std::shared_ptr< ArchiveType > archiveType );

    /// @brief Получение списка записей
    /// @return Список записей архива
    const std::vector< RecordType >& GetRecords() const;

    /// @brief Добавление записи
    /// @param[in] record Добавляемая запись
    /// @note Запись добавляется в список, упорядоченный по возрастанию timestamp
    void AddRecord( RecordType record );

    /// @brief Получение записи
    /// @param[in] index Индекс получаемой записи
    /// @return Получаемая запись или nullptr, если не найдена
    const RecordType GetRecord( size_t index ) const;

    /// @brief Получение записи по метке времени
    /// @param[in] timestamp Временная метка события
    /// @return Получаемая запись или nullptr, если не найдена
    const RecordType GetRecordByTimestamp( TimeType timestamp ) const;

    /// @brief Удаление записи
    /// @param[in] index Индекс удалямой записи
    /// @return Была ли удалена запись
    bool RemoveRecord( size_t index );

    /// @brief Удаление записи
    /// @param[in] record Удаляемая запись
    /// @return Была ли удалена запись
    bool RemoveRecord( RecordType record );

    /// @brief Удаление записи по метке времени
    /// @param[in] timestamp Временная метка события
    /// @return Была ли удалена запись
    bool RemoveRecordByTimestamp( TimeType timestamp );

private:
    std::vector< RecordType > records_; ///< Записи сервисного архива

}; // class ServiceArchive

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_SERVICE_ARCHIVE_H

/// @file Интерфейс класса для работы с соединением
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_ICONNECTION_H
#define LOGIKA_CONNECTIONS_ICONNECTION_H

#include <logika/connections/types.h>
#include <logika/common/types.h>

#include <string>
#include <functional>

namespace logika
{

namespace connections
{

/// @brief Интерфейс класса для работы с соединением
class IConnection
{
public:
    virtual ~IConnection() = default;

    /// @brief Установка соединения
    /// @return Удалось ли установить соединение
    /// @note Сбрасывает текущее соединение, если было установлено
    virtual bool Open() = 0;

    /// @brief Закрытие соединения
    virtual void Close() = 0;

    /// @brief Получение адреса соединения
    /// @return Адрес соединения
    virtual const std::string& GetAddress() const = 0;

    /// @brief Получение времени ожидания данных для чтения
    /// @return Время ожидания данных для чтения, мс
    virtual uint32_t GetReadTimeout() const = 0;

    /// @brief Получение типа соединения
    /// @return Тип соединения
    virtual ConnectionType::Type GetConnectionType() const = 0;

    /// @brief Получение состояния соединения
    /// @return Состояние соединения
    virtual ConnectionState GetConnectionState() const = 0;

    /// @brief Установлено ли соединение
    /// @return Было ли установлено соединение
    virtual bool IsConnected() const = 0;

    /// @brief Получение количества переданных байтов
    /// @return Количество переданных байтов
    virtual uint32_t GetTxBytesCount() const = 0;

    /// @brief Получение количества полученных байтов
    /// @return Количество полученных байтов
    virtual uint32_t GetRxBytesCount() const = 0;

    /// @brief Получение времени последнего чтения
    /// @return Время последнего чтения
    virtual TimeType GetLastRxTime() const = 0;

    /// @brief Очистка заданных буферов
    /// @param[in] flags Типы буферов для очистки
    virtual void Purge( PurgeFlags::Type flags ) = 0;

    /// @brief Установка обработчика события "Подключение установлено"
    /// @param[in] hook Обработчик события
    virtual void SetOnAfterConnect( const std::function< void() >& hook ) = 0;

    /// @brief Установка обработчика события "Соединение будет разорвано"
    /// @param[in] hook Обработчик события
    virtual void SetOnBeforeDisonnect( const std::function< void() >& hook ) = 0;

    /// @todo OnConnectionRequired?

    /// @todo IsConflictsWith?

    /// @brief Сброс статистики по полученным/переданным байтам
    virtual void ResetStatistics() = 0;

    /// @brief Чтение данных
    /// @param[out] buffer Буфер для записи данных
    /// @param[in] needed Желаемое количество байтов
    /// @return Количество прочитанных байтов
    /// @note Количество прочитанных байтов может быть меньше желаемого количества, если соединение было закрыто
    virtual uint32_t Read( ByteVector& buffer, uint32_t needed ) = 0;

    /// @brief Запись данных
    /// @param[in] buffer Буфер, содержащий данные для записи
    /// @return Количество записанных байтов
    /// @note Количество записанных байтов может быть меньше размера буфера, если соединение было закрыто
    virtual uint32_t Write( const ByteVector& buffer ) = 0;
}; // class IConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_ICONNECTION_H

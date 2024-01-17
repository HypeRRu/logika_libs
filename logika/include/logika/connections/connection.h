/// @file Объявление базового класса для работы с соединением
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_CONNECTION_H
#define LOGIKA_CONNECTIONS_CONNECTION_H

#include <logika/connections/iconnection.h>

namespace logika
{

namespace connections
{

/// @brief Базовый класс для работы с соединением
/// @todo Добавить блокировки?
class Connection: public IConnection
{
public:
    /// @brief Конструктор соединения
    /// @param[in] address Адрес соединения
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    Connection( const std::string& address, uint32_t readTimeout = 0 );

    /// @brief Деструктор соединения
    ~Connection();

    /// @brief Установка соединения
    /// @return Удалось ли установить соединение
    /// @note Сбрасывает текущее соединение, если было установлено
    virtual bool Open() override;

    /// @brief Закрытие соединения
    virtual void Close() override;

    /// @brief Получение адреса соединения
    /// @return Адрес соединения
    virtual const std::string& GetAddress() const override;

    /// @brief Получение времени ожидания данных для чтения
    /// @return Время ожидания данных для чтения, мс
    virtual uint32_t GetReadTimeout() const override;

    /// @brief Получение типа соединения
    /// @return Тип соединения
    virtual ConnectionType::Type GetConnectionType() const override;

    /// @brief Получение состояния соединения
    /// @return Состояние соединения
    virtual ConnectionState GetConnectionState() const override;

    /// @brief Установлено ли соединение
    /// @return Было ли установлено соединение
    virtual bool IsConnected() const override;

    /// @brief Получение количества переданных байтов
    /// @return Количество переданных байтов
    virtual uint32_t GetTxBytesCount() const override;

    /// @brief Получение количества полученных байтов
    /// @return Количество полученных байтов
    virtual uint32_t GetRxBytesCount() const override;

    /// @brief Получение времени последнего чтения
    /// @return Время последнего чтения
    virtual TimeType GetLastRxTime() const override;

    /// @brief Очистка заданных буферов
    /// @param[in] flags Типы буферов для очистки
    virtual void Purge( PurgeFlags::Type flags ) override;

    /// @brief Установка обработчика события "Подключение установлено"
    /// @param[in] hook Обработчик события
    virtual void SetOnAfterConnect( const std::function< void() >& hook ) override;

    /// @brief Установка обработчика события "Соединение будет разорвано"
    /// @param[in] hook Обработчик события
    virtual void SetOnBeforeDisonnect( const std::function< void() >& hook ) override;

    /// @brief Сброс статистики по полученным/переданным байтам
    virtual void ResetStatistics() override;

    /// @brief Чтение данных
    /// @param[out] buffer Буфер для записи данных
    /// @param[in] needed Желаемое количество байтов
    /// @return Количество прочитанных байтов
    /// @note Количество прочитанных байтов может быть меньше желаемого количества, если соединение было закрыто
    virtual uint32_t Read( ByteVector& buffer, uint32_t needed ) override;

    /// @brief Запись данных
    /// @param[in] buffer Буфер, содержащий данные для записи
    /// @return Количество записанных байтов
    /// @note Количество записанных байтов может быть меньше размера буфера, если соединение было закрыто
    virtual uint32_t Write( const ByteVector& buffer ) override;

protected:
    /// @brief Реализация установки соединения
    /// @return Удалось ли открыть соединение
    virtual bool OpenImpl();

    /// @brief Реализация закрытия соединения
    virtual void CloseImpl();

    /// @brief Реализация очистки буферов
    /// @param[in] Типы буферов для очистки
    virtual void PurgeImpl( PurgeFlags::Type flags );

    /// @brief Реализация чтения данных
    /// @param[out] buffer Буфер для записи данных
    /// @param[in] needed Желаемое количество байтов
    /// @return Количество прочитанных байтов
    virtual uint32_t ReadImpl( ByteVector& buffer, uint32_t needed );

    /// @brief Реализация записи данных
    /// @param[in] buffer Буфер, содержащий данные для записи
    /// @param[in] start Начиная с какого байта считывать буфер
    /// @return Количество записанных байтов
    virtual uint32_t WriteImpl( const ByteVector& buffer, uint32_t start );

protected:
    const std::string address_;                 ///< Адрес соединения
    uint32_t readTimeout_;                      ///< Время ожидания данных для чтения, мс

    ConnectionType::Type type_;                 ///< Тип соединения
    ConnectionState state_;                     ///< Состояние соединения

    uint32_t txBytesCount_;                     ///< Количество переданных байтов
    uint32_t rxBytesCount_;                     ///< Количество полученных байтов

    TimeType lastRxTime_;                       ///< Время последнего чтения

    std::function< void() > onAfterConnect_;    ///< Обработчик установленного соединения
    std::function< void() > onBeforeDisonnect_; ///< Обработчик разрыва соединения

}; // class Connection

} // namespace logika

} // namespace connections

#endif // LOGIKA_CONNECTIONS_CONNECTION_H

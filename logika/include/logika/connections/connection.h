/// @file Объявление базового класса для работы с соединением
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_CONNECTION_H
#define LOGIKA_CONNECTIONS_CONNECTION_H

#include <logika/connections/defs.h>
#include <logika/connections/iconnection.h>

#include <logika/connections/rc.h>

namespace logika
{

namespace connections
{

/// @brief Базовый класс для работы с соединением
class LOGIKA_CONNECTIONS_EXPORT Connection: public IConnection
{
public:
    /// @brief Конструктор соединения
    /// @param[in] address Адрес соединения
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    Connection( const std::string& address, TimeType readTimeout = 0 );

    /// @brief Деструктор соединения
    ~Connection();

    Connection( const Connection& ) = delete;
    Connection& operator =( const Connection& ) = delete;

    /// @copydoc IConnection::Open()
    virtual bool Open() override;

    /// @copydoc IConnection::Close()
    virtual void Close() override;

    /// @copydoc IConnection::GetAddress()
    virtual const std::string& GetAddress() const override;

    /// @copydoc IConnection::GetReadTimeout()
    virtual TimeType GetReadTimeout() const override;

    /// @copydoc IConnection::SetReadTimeout()
    virtual void SetReadTimeout( TimeType timeout ) override;

    /// @copydoc IConnection::GetConnectionType()
    virtual ConnectionType::Type GetConnectionType() const override;

    /// @copydoc IConnection::GetConnectionState()
    virtual ConnectionState GetConnectionState() const override;

    /// @copydoc IConnection::IsConnected()
    virtual bool IsConnected() const override;

    /// @copydoc IConnection::GetTxBytesCount()
    virtual uint32_t GetTxBytesCount() const override;

    /// @copydoc IConnection::GetRxBytesCount()
    virtual uint32_t GetRxBytesCount() const override;

    /// @copydoc IConnection::GetLastRxTime()
    virtual TimeType GetLastRxTime() const override;

    /// @copydoc IConnection::Purge()
    virtual void Purge( PurgeFlags::Type flags ) override;

    /// @copydoc IConnection::SetOnAfterConnect()
    virtual void SetOnAfterConnect( const std::function< void() >& hook ) override;

    /// @copydoc IConnection::SetOnBeforeDisonnect()
    virtual void SetOnBeforeDisonnect( const std::function< void() >& hook ) override;

    /// @copydoc IConnection::ResetStatistics()
    virtual void ResetStatistics() override;

    /// @copydoc IConnection::Read()
    virtual uint32_t Read( ByteVector& buffer, uint32_t needed ) override;

    /// @copydoc IConnection::Write()
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

    /// @brief Реализация чтения части данных
    /// @param[out] buffer Буфер для записи данных
    /// @param[in] start Начиная с какого байта записывать в буфер
    /// @param[in] needed Желаемое количество байтов
    /// @param[out] rc Статус операции. Rc::Success в случае успеха.
    /// @return Количество прочитанных байтов
    /// @note После первого чтения возвращает количество прочитанных байтов
    virtual uint32_t ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed, Rc::Type* rc = nullptr );

    /// @brief Реализация записи данных
    /// @param[in] buffer Буфер, содержащий данные для записи
    /// @param[in] start Начиная с какого байта считывать буфер
    /// @param[out] rc Статус операции. Rc::Success в случае успеха.
    /// @return Количество записанных байтов
    /// @note После первой записи возвращает количество записанных байтов
    virtual uint32_t WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc = nullptr );

protected:
    const std::string address_;                 ///< Адрес соединения
    const LocString locAddress_;                ///< Локализованный адрес соединения (для логирования)
    TimeType readTimeout_;                      ///< Время ожидания данных для чтения, мс

    ConnectionType::Type type_;                 ///< Тип соединения
    ConnectionState state_;                     ///< Состояние соединения

    uint32_t txBytesCount_;                     ///< Количество переданных байтов
    uint32_t rxBytesCount_;                     ///< Количество полученных байтов

    TimeType lastRxTime_;                       ///< Метка времени последнего чтения (мс)

    std::function< void() > onAfterConnect_;    ///< Обработчик установленного соединения
    std::function< void() > onBeforeDisonnect_; ///< Обработчик разрыва соединения

}; // class Connection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_CONNECTION_H

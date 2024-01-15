/// @file Объявление базового класса для работы с соединением
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_CONNECTION_H
#define LOGIKA_CONNECTIONS_CONNECTION_H

#include <logika/connections/iconnection.h>

#include <iostream>

#define LOG_ERROR 0
#define LOG_WARNING 1
#define LOG_INFO 2
#define LOG_DEBUG 3
#define LOG( level, message ) do { std::cerr << '[' << ( level ) << ']' << message << std::endl; } while ( false )

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
    Connection( const std::string& address, uint32_t readTimeout = 0 )
        : address_{ address }
        , readTimeout_{ readTimeout }
        , type_{ ConnectionType::Offline }
        , state_{ ConnectionState::NotConnected }
        , txBytesCount_{ 0 }
        , rxBytesCount_{ 0 }
        , lastRxTime_{ 0 }
        , onAfterConnect_{}
        , onBeforeDisonnect_{}
    {
        Connect();
    } // Connection

    /// @brief Деструктор соединения
    ~Connection()
    {
        Close(); 
    } // ~Connection

    /// @brief Установка соединения
    /// @return Удалось ли установить соединение
    /// @note Сбрасывает текущее соединение, если было установлено
    virtual bool Connect() override
    {
        if ( ConnectionState::Connected == state_ )
        {
            LOG( LOG_WARNING, "Found active connection to " << address_ << ". Closing" );
            Close();
        }
        LOG( LOG_INFO, "Connecting to " << address_ );
        state_ = ConnectionState::Connecting;
        if ( ConnectImpl() )
        {
            LOG( LOG_INFO, "Successfully connected to " << address_ );
            state_ = ConnectionState::Connected;
            onAfterConnect_();
            return true;
        }
        else
        {
            LOG( LOG_ERROR, "Failed to connect to " << address_ );
            state_ = ConnectionState::NotConnected;
            return false;
        }
    }

    /// @brief Закрытие соединения
    virtual void Close() override
    {
        if (   ConnectionState::Connected  == state_ 
            || ConnectionState::Connecting == state_ )
        {
            LOG( LOG_INFO, "Disconnecting from " << address_ );
            state_ = ConnectionState::Disconnecting;
            onBeforeDisonnect_();
            CloseImpl();
            state_ = ConnectionState::NotConnected;
        }
    } // Close

    /// @brief Получение адреса соединения
    /// @return Адрес соединения
    virtual const std::string& GetAddress() const override
    {
        return address_;
    } // GetAddress

    /// @brief Получение времени ожидания данных для чтения
    /// @return Время ожидания данных для чтения, мс
    virtual uint32_t GetReadTimeout() const override
    {
        return readTimeout_;
    } // GetReadTimeout

    /// @brief Получение типа соединения
    /// @return Тип соединения
    virtual ConnectionType::Type GetConnectionType() const override
    {
        return type_;
    } // GetConnectionType

    /// @brief Получение состояния соединения
    /// @return Состояние соединения
    virtual ConnectionState GetConnectionState() const override
    {
        return state_;
    } // GetConnectionState

    /// @brief Установлено ли соединение
    /// @return Было ли установлено соединение
    virtual bool IsConnected() const override
    {
        return state_ == ConnectionState::Connected;
    } // IsConnected

    /// @brief Получение количества переданных байтов
    /// @return Количество переданных байтов
    virtual uint32_t GetTxBytesCount() const override
    {
        return txBytesCount_;
    } // GetTxBytesCount

    /// @brief Получение количества полученных байтов
    /// @return Количество полученных байтов
    virtual uint32_t GetRxBytesCount() const override
    {
        return rxBytesCount_;
    } // GetRxBytesCount

    /// @brief Получение времени последнего чтения
    /// @return Время последнего чтения
    virtual TimeType GetLastRxTime() const override
    {
        return lastRxTime_;
    } // GetLastRxTime

    /// @brief Очистка заданных буферов
    /// @param[in] flags Типы буферов для очистки
    virtual void Purge( PurgeFlags::Type flags ) override
    {
        /// @todo PurgeFlags to string converter
        if ( IsConnected() )
        {
            LOG( LOG_INFO, "Purge" << ( ( flags & PurgeFlags::Rx ) ? " RX" : "" )
                << ( ( flags & PurgeFlags::Rx & PurgeFlags::Tx ) ? " and" : "" )
                << ( ( flags & PurgeFlags::Tx ) ? " TX" : "" ) );
            PurgeImpl( flags );
        }
    } // Purge

    /// @brief Установка обработчика события "Подключение установлено"
    /// @param[in] hook Обработчик события
    virtual void SetOnAfterConnect( const std::function< void() >& hook ) override
    {
        LOG( LOG_INFO, "Set OnAfterConnect hook" );
        onAfterConnect_ = hook;
    } // SetOnAfterConnect

    /// @brief Установка обработчика события "Соединение будет разорвано"
    /// @param[in] hook Обработчик события
    virtual void SetOnBeforeDisonnect( const std::function< void() >& hook ) override
    {
        LOG( LOG_INFO, "Set OnBeforeDisconnect hook" );
        onBeforeDisonnect_ = hook;
    } // SetOnBeforeDisonnect

protected:
    virtual bool ConnectImpl()
    {
        /// @todo debug
        return false;
    } // ConnectImpl

    /// @brief Реализация закрытия соединения
    virtual void CloseImpl()
    {
        /// @todo debug
    } // CloseImpl

    /// @brief Реализация очистки буферов
    /// @param[in] Типы буферов для очистки
    virtual void PurgeImpl( PurgeFlags::Type flags )
    {
        /// @todo debug
    } // PurgeImpl

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

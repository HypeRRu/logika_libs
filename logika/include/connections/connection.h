/// @file Объявление базового класса для работы с соединением
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_CONNECTION_H
#define LOGIKA_CONNECTIONS_CONNECTION_H

#include <connections/iconnection.h>

namespace logika
{

namespace connections
{

/// @brief Базовый класс для работы с соединением
class Connection: public IConnection
{
public:
    /// @brief Конструктор соединения
    /// @param[in] address Адрес соединения
    Connection( const std::string& address )
        : address_{ address }
        , type_{ ConnectionType::Offline }
        , state_{ ConnectionState::NotConnected }
    {} // Connection

    /// @brief Получение адреса соединения
    /// @return Адрес соединения
    virtual const std::string& GetAddress() const override
    {
        return address_;
    } // GetAddress

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

protected:
    const std::string address_;         ///< Адрес соединения

    ConnectionType::Type type_;         ///< Тип соединения
    ConnectionState state_;             ///< Состояние соединения

    uint32_t txBytesCount_;             ///< Количество полученных байтов TX
    uint32_t rxBytexCount_;             ///< Количество полученных байтов RX

}; // class Connection

} // namespace logika

} // namespace connections

#endif // LOGIKA_CONNECTIONS_CONNECTION_H

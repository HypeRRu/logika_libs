/// @file Объявление базового класса для работы с соединением по сети
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_NETWORK_NET_CONNECTION_H
#define LOGIKA_CONNECTIONS_NETWORK_NET_CONNECTION_H

#include <logika/connections/connection.h>

namespace logika
{

namespace connections
{

/// @brief Базовый класс для работы с соединением по сети
class NetConnection: public Connection
{
public:
    /// @brief Конструктор соединения по сети
    /// @param[in] serverHostName Адрес сервера
    /// @param[in] serverPort Порт сервера
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    NetConnection( const std::string& serverHostName, uint16_t serverPort, uint32_t readTimeout = 0 );

    /// @brief Деструктор соединения
    ~NetConnection() = default;
    
protected:
    std::string serverHostName_;    ///< Адрес сервера
    uint16_t serverPort_;           ///< Порт сервера

}; // class NetConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_NETWORK_NET_CONNECTION_H

/// @file Интерфейс класса для работы с соединением
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_ICONNECTION_H
#define LOGIKA_CONNECTIONS_ICONNECTION_H

#include <connections/types.h>

#include <string>

namespace logika
{

namespace connections
{

/// @brief Интерфейс класса для работы с соединением
class IConnection
{
public:
    virtual ~IConnection() = default;

    /// @brief Получение адреса соединения
    /// @return Адрес соединения
    virtual const std::string& GetAddress() const = 0;

    /// @brief Получение типа соединения
    /// @return Тип соединения
    virtual ConnectionType::Type GetConnectionType() const = 0;

    /// @brief Получение состояния соединения
    /// @return Состояние соединения
    virtual ConnectionState GetConnectionState() const = 0;
}; // class IConnection

} // namespace logika

} // namespace connections

#endif // LOGIKA_CONNECTIONS_ICONNECTION_H

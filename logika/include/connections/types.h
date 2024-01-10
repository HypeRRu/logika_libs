/// @file Определение типов соединений
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_TYPES_H
#define LOGIKA_CONNECTIONS_TYPES_H

#include <cstdint>

namespace logika
{

namespace connections
{

namespace ConnectionType
{

using Type = uint16_t;

/// @brief Тип соединения
enum : Type
{
    BaseTypeShift   = 4,

    Offline         = 0,                                    ///< Тип соединения не задан
    Serial          = 1 << 0,                               ///< Последовательное соединение
    Network         = 1 << 1,                               ///< Соединение по сети

    ComPort         = ( 0x01 << BaseTypeShift ) | Serial,   ///< ( 0x11; 17 ) Соединение с использованием COM-порта
    Tcp             = ( 0x02 << BaseTypeShift ) | Network,  ///< ( 0x22; 34 ) Соединение по TCP
    Udp             = ( 0x03 << BaseTypeShift ) | Network,  ///< ( 0x32; 50 ) Соединение по UDP
    Modem           = ( 0x04 << BaseTypeShift ) | Network,  ///< ( 0x42; 66 ) Соединение с использованием модема
    Radius          = ( 0x05 << BaseTypeShift ) | Network,  ///< ( 0x52; 82 ) Соединение с использованием RADIUS-сервера
}; // anonymous enum

} // namespace ConnectionType


/// @brief Состояние соединения
enum class ConnectionState
{
    NotConnected,   ///< Соединение не установлено
    Connecting,     ///< Выполняется подключение
    Connected,      ///< Соединение установлено
    Disconnecting   ///< Выполняется отключение
}; // enum class ConnectionState


/// @brief Флаги очистки
enum class PurgeFlags
{
    Rx,
    Tx
}; // enum class PurgeFlags

} // namespace logika

} // namespace connections

#endif // LOGIKA_CONNECTIONS_TYPES_H

/// @file Объявление базового класса для работы с соединением по последовательному порту
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_SERIAL_SERIAL_CONNECTION_H
#define LOGIKA_CONNECTIONS_SERIAL_SERIAL_CONNECTION_H

#include <logika/connections/defs.h>
#include <logika/connections/connection.h>
#include <logika/connections/serial/types.h>

namespace logika
{

namespace connections
{

/// @brief Базовый класс для работы с соединением по последовательному порту
class LOGIKA_CONNECTIONS_EXPORT SerialConnection: public Connection
{
public:
    /// @brief Конструктор соединения по последовательному порту
    /// @param[in] portName Имя последовательного порта
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    SerialConnection( const std::string& portName, uint32_t readTimeout = 0 );

    /// @brief Получение скорости работы, бит/c
    /// @return Скорость работы
    BaudRate::Type GetBaudRate() const;

    /// @brief Задание скорости работы
    /// @param[in] rate Скорость работы, бит/с
    /// @return Валидность данных
    /// @note При невалидных данных соединение сбрасывается и параметр устанавливается в NotSupported
    /// @note Если соединение было открыто, оно переоткрывается
    bool SetBaudRate( BaudRate::Type rate );

    /// @brief Получение количества используемых стоп-битов
    /// @return Количество используемых стоп-битов
    StopBits::Type GetStopBits() const;

    /// @brief Задание количества используемых стоп-битов
    /// @param[in] bits Количество используемых стоп-битов
    /// @return Валидность данных
    /// @note При невалидных данных соединение сбрасывается и параметр устанавливается в NotSupported
    /// @note Если соединение было открыто, оно переоткрывается
    bool SetStopBits( StopBits::Type bits );

    /// @brief Получение количества используемых битов данных
    /// @return Количество используемых битов данных
    DataBits::Type GetDataBits() const;

    /// @brief Задание количества используемых битов данных
    /// @param[in] bits Количество используемых битов данных
    /// @return Валидность данных
    /// @note При невалидных данных соединение сбрасывается и параметр устанавливается в NotSupported
    /// @note Если соединение было открыто, оно переоткрывается
    bool SetDataBits( DataBits::Type bits );

    /// @brief Получение схемы четности
    /// @return Схема четности
    Parity::Type GetParity() const;

    /// @brief Задание схемы четности
    /// @param[in] parity Схема четности
    /// @return Валидность данных
    /// @note При невалидных данных соединение сбрасывается и параметр устанавливается в NotSupported
    /// @note Если соединение было открыто, оно переоткрывается
    bool SetParity( Parity::Type parity );

protected:
    BaudRate::Type  baudRate_;  ///< Скорость работы порта
    StopBits::Type  stopBits_;  ///< Количество используемых стоп-битов
    DataBits::Type  dataBits_;  ///< Количество битов данных
    Parity::Type    parity_;    ///< Схема четности

}; // class SerialConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_SERIAL_SERIAL_CONNECTION_H

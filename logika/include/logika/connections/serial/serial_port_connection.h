/// @file Объявление класса для работы с соединением по COM-порту
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_SERIAL_SERIAL_PORT_CONNECTION_H
#define LOGIKA_CONNECTIONS_SERIAL_SERIAL_PORT_CONNECTION_H

#include <logika/connections/serial/serial_connection.h>

namespace logika
{

namespace connections
{

/// @brief Класс для работы с соединением COM-порту
class SerialPortConnection final: public SerialConnection 
{
public:
    /// @brief Конструктор соединения по последовательному порту
    /// @param[in] portName Имя последовательного порта
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    SerialPortConnection( const std::string& portName, uint32_t readTimeout = 0 );

    /// @brief Конструктор соединения по последовательному порту
    /// @param[in] portName Имя последовательного порта
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    SerialPortConnection( const std::string& portName, BaudRate::Type baudRate
        , StopBits::Type stopBits, DataBits::Type dataBits
        , Parity::Type parity, uint32_t readTimeout = 0 );

    /// @brief Деструктор соединения
    ~SerialPortConnection();

    /// @todo available bytes

protected:
    /// @brief Реализация установки соединения
    /// @return Удалось ли открыть соединение
    bool OpenImpl() override;

    /// @brief Реализация закрытия соединения
    void CloseImpl() override;

    /// @brief Реализация очистки буферов
    /// @param[in] Типы буферов для очистки
    void PurgeImpl( PurgeFlags::Type flags ) override;

    /// @brief Реализация чтения части данных
    /// @param[out] buffer Буфер для записи данных
    /// @param[in] start Начиная с какого байта записывать в буфер
    /// @param[in] needed Желаемое количество байтов
    /// @return Количество прочитанных байтов
    /// @note После первого чтения возвращает количество прочитанных байтов
    uint32_t ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed ) override;

    /// @brief Реализация записи данных
    /// @param[in] buffer Буфер, содержащий данные для записи
    /// @param[in] start Начиная с какого байта считывать буфер
    /// @return Количество записанных байтов
    uint32_t WriteImpl( const ByteVector& buffer, uint32_t start ) override;

private:
    FileHandleType handle_;

}; // class SerialPortConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_SERIAL_SERIAL_PORT_CONNECTION_H

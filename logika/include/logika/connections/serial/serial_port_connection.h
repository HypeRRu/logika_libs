/// @file Объявление класса для работы с соединением по COM-порту
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_SERIAL_SERIAL_PORT_CONNECTION_H
#define LOGIKA_CONNECTIONS_SERIAL_SERIAL_PORT_CONNECTION_H

#include <logika/connections/defs.h>
#include <logika/connections/serial/serial_connection.h>

namespace logika
{

namespace connections
{

/// @brief Класс для работы с соединением COM-порту
class LOGIKA_CONNECTIONS_EXPORT SerialPortConnection final: public SerialConnection 
{
public:
    /// @brief Конструктор соединения по последовательному порту
    /// @param[in] portName Имя последовательного порта
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    SerialPortConnection( const std::string& portName, TimeType readTimeout = 0 );

    /// @brief Конструктор соединения по последовательному порту
    /// @param[in] portName Имя последовательного порта
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    SerialPortConnection( const std::string& portName, BaudRate::Type baudRate
        , StopBits::Type stopBits, DataBits::Type dataBits
        , Parity::Type parity, TimeType readTimeout = 0 );

    /// @brief Деструктор соединения
    ~SerialPortConnection();

    /// @brief Получение количества доступных для чтения байтов
    /// @return Количество доступных для чтения байтов или -1 в случае ошибки
    int32_t BytesAvailable();

    /// @brief Проверка валидности настроек соединения
    /// @return Валидны ли параметры соединения
    bool IsSettingsValid() const;

protected:
    /// @copydoc Connection::OpenImpl()
    bool OpenImpl() override;

    /// @copydoc Connection::CloseImpl()
    void CloseImpl() override;

    /// @copydoc Connection::PurgeImpl()
    void PurgeImpl( PurgeFlags::Type flags ) override;

    /// @copydoc Connection::ReadImpl()
    uint32_t ReadImpl( ByteVector& buffer, uint32_t start, uint32_t needed, Rc::Type* rc = nullptr ) override;

    /// @copydoc Connection::WriteImpl()
    uint32_t WriteImpl( const ByteVector& buffer, uint32_t start, Rc::Type* rc = nullptr ) override;

private:
    FileHandleType handle_; ///< Дескриптор соединения

}; // class SerialPortConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_SERIAL_SERIAL_PORT_CONNECTION_H

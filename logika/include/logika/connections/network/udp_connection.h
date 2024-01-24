/// @file Объявление класса для работы с соединением по UDP
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_NETWORK_UDP_CONNECTION_H
#define LOGIKA_CONNECTIONS_NETWORK_UDP_CONNECTION_H

#include <logika/connections/network/net_connection.h>

namespace logika
{

namespace connections
{

/// @brief Класс для работы с соединением по UDP
class UdpConnection: public NetConnection
{
public:
    /// @brief Конструктор UDP-соединения
    /// @param[in] serverHostName Адрес сервера
    /// @param[in] serverPort Порт сервера
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    UdpConnection( const std::string& serverHostName, uint16_t serverPort, uint32_t readTimeout = 0 );
    
    /// @brief Деструктор UDP-соединения
    ~UdpConnection();

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
    SocketType  socket_; ///< Сокет UDP-соединения

}; // class UdpConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_NETWORK_UDP_CONNECTION_H

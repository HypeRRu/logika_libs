/// @file Объявление класса для работы с соединением по UDP
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_NETWORK_UDP_CONNECTION_H
#define LOGIKA_CONNECTIONS_NETWORK_UDP_CONNECTION_H

#include <logika/connections/defs.h>
#include <logika/connections/network/net_connection.h>

namespace logika
{

namespace connections
{

/// @brief Класс для работы с соединением по UDP
class LOGIKA_CONNECTIONS_EXPORT UdpConnection: public NetConnection
{
public:
    /// @brief Конструктор UDP-соединения
    /// @param[in] serverHostName Адрес сервера
    /// @param[in] serverPort Порт сервера
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    UdpConnection( const std::string& serverHostName, uint16_t serverPort, TimeType readTimeout = 0 );
    
    /// @brief Деструктор UDP-соединения
    ~UdpConnection();

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
    SocketType  socket_; ///< Сокет UDP-соединения

}; // class UdpConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_NETWORK_UDP_CONNECTION_H

/// @file Объявление класса для работы с соединением по TCP
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_NETWORK_TCP_CONNECTION_H
#define LOGIKA_CONNECTIONS_NETWORK_TCP_CONNECTION_H

#include <logika/connections/defs.h>
#include <logika/connections/network/net_connection.h>

namespace logika
{

namespace connections
{

/// @brief Класс для работы с соединением по TCP
class LOGIKA_CONNECTIONS_EXPORT TcpConnection: public NetConnection
{
public:
    /// @brief Конструктор TCP-соединения
    /// @param[in] serverHostName Адрес сервера
    /// @param[in] serverPort Порт сервера
    /// @param[in] readTimeout Время ожидания данных для чтения, мс. По умолчанию 0 - не ограничено.
    TcpConnection( const std::string& serverHostName, uint16_t serverPort, uint32_t readTimeout = 0 );
    
    /// @brief Деструктор TCP-соединения
    ~TcpConnection();

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
    SocketType  socket_; ///< Сокет TCP-соединения

}; // class TcpConnection

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_NETWORK_TCP_CONNECTION_H
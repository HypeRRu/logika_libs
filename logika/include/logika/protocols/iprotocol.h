/// @file Интерфейс класса для работы с протоколами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_IPROTOCOL_H
#define LOGIKA_PROTOCOLS_IPROTOCOL_H

#include <logika/connections/iconnection.h>
#include <logika/protocols/rc.h>

#include <memory>

namespace logika
{

namespace connections
{
    class IConnection; ///< forward declaration интерфейса соединения
} // namespace connections;

namespace protocols
{

/// @brief Интерфейс класса для работы с протоколами
class IProtocol
{
public:
    virtual ~IProtocol() = default;

    /// @brief Получение соединения для работы протокола
    /// @return Соединение для работы протокола
    virtual std::shared_ptr< connections::IConnection > GetConnection() const = 0;

    /// @brief Задание соединения для работы протокола
    /// @param[in] connection Соединение для работы протокола. Для сброса можно передать nullptr
    virtual void SetConnection( std::shared_ptr< connections::IConnection > connection ) = 0;

    /// @brief Получение количества отправленных пакетов
    /// @return Количество отправленных пакетов
    virtual uint32_t GetPacketsSent() const = 0;

    /// @brief Получение количества полученных пакетов
    /// @return Количество полученных пакетов
    virtual uint32_t GetPacketsReceived() const = 0;

    /// @brief Получение количества ошибок TimeOut
    /// @return Количество ошибок TimeOut
    virtual uint32_t GetRxTimeouts() const = 0;

    /// @brief Получение количества ошибок CRC
    /// @return Количество ошибок CRC
    virtual uint32_t GetRxCrc() const = 0;

    /// @brief Получение количества ошибок LatePacket
    /// @return Количество ошибок LatePacket
    virtual uint32_t GetRxLatePackets() const = 0;

    /// @brief Получение количества неклассифицированных ошибок получения
    /// @return Количество неклассифицированных ошибок получения
    virtual uint32_t GetRxGeneralErrors() const = 0;

    /// @brief Получение количества неклассифицированных ошибок
    /// @return Количество неклассифицированных ошибок
    virtual uint32_t GetGeneralErrors() const = 0;

    /// @brief Регистрация события
    /// @param[in] event Тип события
    virtual void RegisterEvent( Rc::Type event ) = 0;

    /// @brief Сброс статистики событий
    virtual void Reset() = 0;

    /// @todo CloseCommSession?
    /// @todo Архив, автодетект и т.д
}; // class IProtocol

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_IPROTOCOL_H

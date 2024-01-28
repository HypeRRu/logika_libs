/// @file Объявление базового класса для работы с протоколами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_PROTOCOL_H
#define LOGIKA_PROTOCOLS_PROTOCOL_H

#include <logika/protocols/iprotocol.h>

namespace logika
{

namespace protocols
{

/// @brief Базовый класс для работы с протоколами
class Protocol: public IProtocol
{
public:
    /// @brief Конструктор протокола
    Protocol();

    /// @copydoc IProtocol::GetConnection()
    virtual std::shared_ptr< connections::IConnection > GetConnection() const override;

    /// @copydoc IProtocol::SetConnection()
    virtual void SetConnection( std::shared_ptr< connections::IConnection > connection ) override;

    /// @copydoc IProtocol::GetPacketsSent()
    virtual uint32_t GetPacketsSent() const override;

    /// @copydoc IProtocol::GetPacketsReceived()
    virtual uint32_t GetPacketsReceived() const override;

    /// @copydoc IProtocol::GetRxTimeouts()
    virtual uint32_t GetRxTimeouts() const override;

    /// @copydoc IProtocol::GetRxCrc()
    virtual uint32_t GetRxCrc() const override;

    /// @copydoc IProtocol::GetRxLatePackets()
    virtual uint32_t GetRxLatePackets() const override;

    /// @copydoc IProtocol::GetRxGeneralErrors()
    virtual uint32_t GetRxGeneralErrors() const override;

    /// @copydoc IProtocol::GetGeneralErrors()
    virtual uint32_t GetGeneralErrors() const = 0;

    /// @copydoc IProtocol::GetRxGeneralErrors()
    virtual void RegisterEvent( Rc::Type event ) override;

    /// @copydoc IProtocol::Reset()
    virtual void Reset() override;

protected:
    /// @brief Реализация сброса статистики
    virtual void ResetImpl();

protected:
    std::shared_ptr< connections::IConnection > connection_;    ///< Соединения для работы по протоколу

    uint32_t packetsSent_;                                      ///< Количество отправленных пакетов
    uint32_t packetsReceived_;                                  ///< Количество полученных пакетов
    uint32_t rxTimeout_;                                        ///< Количество ошибок чтения TimeOut
    uint32_t rxCrc_;                                            ///< Количество ошибок чтения CRC
    uint32_t rxLatePacket_;                                     ///< Количество ошибок чтения Late Packet
    uint32_t rxGeneral_;                                        ///< Количество неклассифицированных ошибок чтения
    uint32_t generalError_;                                     ///< Количество неклассифицированных ошибок

}; // class Protocol

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_PROTOCOL_H

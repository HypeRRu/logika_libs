/// @file Объявление базового класса для работы с протоколами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_PROTOCOL_H
#define LOGIKA_PROTOCOLS_PROTOCOL_H

#include <logika/protocols/defs.h>
#include <logika/protocols/iprotocol.h>

#include <logika/common/types.h>
#include <logika/connections/types.h>
#include <logika/connections/serial/types.h>
#include <logika/meters/types.h>
#include <logika/meters/meter.h>
#include <logika/storage/storage.hpp>

/// @cond
#include <mutex>
#include <condition_variable>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{
class LOGIKA_PROTOCOLS_EXPORT M4Protocol;
} // namespace M4

namespace X6
{
class LOGIKA_PROTOCOLS_EXPORT SPBusProtocol;
} // namespace X6

/// @brief Базовый класс для работы с протоколами
class LOGIKA_PROTOCOLS_EXPORT Protocol: public IProtocol
{
public:
    /// @brief Конструктор протокола
    /// @param[in] sKeeper Хранилище приложения
    Protocol( const storage::StorageKeeper& sKeeper );

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
    virtual uint32_t GetGeneralErrors() const override;

    /// @copydoc IProtocol::GetRxGeneralErrors()
    virtual void RegisterEvent( Rc::Type event ) override;

    /// @copydoc IProtocol::Reset()
    virtual void Reset() override;

    /// @copydoc IProtocol::CloseCommSession()
    virtual void CloseCommSession( const ByteType* srcNt, const ByteType* dstNt ) override;

    /// @copydoc IProtocol::WaitFor()
    virtual bool WaitFor( TimeType duration ) override;

    /// @copydoc IProtocol::CancelWait()
    virtual void CancelWait() override;

    /// @copydoc IProtocol::GetMeterType()
    virtual std::shared_ptr< meters::Meter > GetMeterType( const ByteType* srcNt,
        const ByteType* dstNt, LocString& extraData ) override;

    /// @copydoc IProtocol::UpdateTags()
    virtual void UpdateTags( const ByteType* srcNt, const ByteType* dstNt,
        std::vector< std::shared_ptr< meters::DataTag > >& tags ) override;

    /// @copydoc IProtocol::ReadIntervalArchiveDef()
    virtual std::shared_ptr< meters::IntervalArchive > ReadIntervalArchiveDef( std::shared_ptr< meters::Meter > meter,
        const ByteType* srcNt, const ByteType* dstNt, std::shared_ptr< meters::ArchiveType > archiveType,
        std::shared_ptr< logika::Any >& state ) override;

    /// @copydoc IProtocol::ReadIntervalArchive()
    virtual bool ReadIntervalArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
        const ByteType* dstNt, std::shared_ptr< meters::IntervalArchive > archive, TimeType start, TimeType end,
        std::shared_ptr< logika::Any >& state, double& progress ) override;

    /// @copydoc IProtocol::ReadServiceArchive()
    virtual bool ReadServiceArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
        const ByteType* dstNt, std::shared_ptr< meters::ServiceArchive > archive, TimeType start, TimeType end,
        std::shared_ptr< logika::Any >& state, double& progress ) override;

    /// @copydoc IProtocol::GetDeviceClock()
    virtual TimeType GetDeviceClock( std::shared_ptr< meters::Meter > meter,
        const ByteType* srcNt, const ByteType* dstNt ) override;

public:
    /// @brief Определение модели прибора на шине M4
    /// @param[in] bus Шина
    /// @param[in] sKeeper Хранилище приложения
    /// @param[out] dump Дамп ответа от прибора
    /// @param[out] model Модификация прибора
    /// @return Модель прибора. nullptr, если не распознан
    static std::shared_ptr< meters::Meter > DetectM4( std::shared_ptr< M4::M4Protocol > bus,
        const storage::StorageKeeper& sKeeper, ByteVector& dump, LocString& model );
    
    /// @brief Определение модели прибора на шине SPBus
    /// @param[in] bus Шина
    /// @param[in] sKeeper Хранилище приложения
    /// @param[out] dump Дамп ответа от прибора
    /// @param[out] model Модификация прибора
    /// @return Модель прибора. nullptr, если не распознан
    static std::shared_ptr< meters::Meter > DetectX6( std::shared_ptr< X6::SPBusProtocol > bus,
        const storage::StorageKeeper& sKeeper, ByteVector& dump, LocString& model );

    /// @brief Определение модели прибора
    /// @param[in] connection Соединение с прибором
    /// @param[in] sKeeper Хранилище приложения
    /// @param[in] fixedBaudRate Фиксированное значение BaudRate
    /// @param[in] waitTimeout Время ожидания ответа
    /// @param[in] tryM4 Проверка приборов на шине M4
    /// @param[in] trySpBus Проверка приборов на шине SPBus
    /// @param[in] tryMek Проверка приборов MEK на шине SPBus
    /// @param[in] srcAddr Адрес источника
    /// @param[in] dstAddr Адрес назначения
    /// @param[out] dump Дамп ответа от прибора
    /// @param[out] deviceBaudRate Детектированное значение BaudRate
    /// @param[out] model Модификация прибора
    /// @return Модель прибора. nullptr, если не распознан
    static std::shared_ptr< meters::Meter > AutodectSpt( std::shared_ptr< connections::IConnection > connection,
        const storage::StorageKeeper& sKeeper, connections::BaudRate::Type fixedBaudRate,
        uint32_t waitTimeout, bool tryM4, bool trySpBus, bool tryMek,
        const ByteType* srcAddr, const ByteType* dstAddr, ByteVector& dump,
        connections::BaudRate::Type& deviceBaudRate, LocString& model );

    /// @brief Получение стандартного времени ожидания чтения (мс)
    /// @param[in] proto Тип шины
    /// @param[in] connType Тип соединения
    /// @return Стандартное время ожидания чтения
    static uint32_t GetDefaultTimeout( meters::BusProtocolType proto, connections::ConnectionType::Type connType );

    /// @brief Расчет контрольной суммы CRC16
    /// @param[in] crc Контрольная сумма CRC16, расчитанная ранее (для предыдущих блоков)
    /// @param[in] buffer Буфер с данными
    /// @param[in] offset Отступ до данных
    /// @param[in] length Длина данных
    /// @return Значение контрольной суммы CRC16
    static uint16_t Crc16( uint16_t crc, const ByteVector& buffer, uint32_t offset, uint32_t length );

protected:
    /// @brief Сброс внутреннего состояния шины
    virtual void ResetBusActiveState();

    /// @brief Реализация закрытия сессии соединения
    /// @param[in] srcNt NT отправителя
    /// @param[in] dstNt NT получателя
    virtual void CloseCommSessionImpl( const ByteType* srcNt, const ByteType* dstNt );

protected:
    std::shared_ptr< connections::IConnection > connection_;    ///< Соединения для работы по протоколу

    uint32_t packetsSent_;                                      ///< Количество отправленных пакетов
    uint32_t packetsReceived_;                                  ///< Количество полученных пакетов
    uint32_t rxTimeout_;                                        ///< Количество ошибок чтения TimeOut
    uint32_t rxCrc_;                                            ///< Количество ошибок чтения CRC
    uint32_t rxLatePacket_;                                     ///< Количество ошибок чтения Late Packet
    uint32_t rxGeneral_;                                        ///< Количество неклассифицированных ошибок чтения
    uint32_t generalError_;                                     ///< Количество неклассифицированных ошибок
    
    std::mutex waitMtx_;                                        ///< Мьютекс для отменяемого ожидания
    std::condition_variable waitCond_;                          ///< Условная переменная для отменяемого ожидания

    const storage::StorageKeeper& sKeeper_;                     ///< Хранилище приложения

}; // class Protocol

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_PROTOCOL_H

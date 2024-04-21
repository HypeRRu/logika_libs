/// @file Интерфейс класса для работы с протоколами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_IPROTOCOL_H
#define LOGIKA_PROTOCOLS_IPROTOCOL_H

#include <logika/protocols/defs.h>
#include <logika/protocols/rc.h>

#include <logika/common/types.h>
#include <logika/connections/defs.h>
#include <logika/meters/defs.h>
#include <logika/common/any.hpp>

/// @cond
#include <memory>
/// @endcond

namespace logika
{

namespace connections
{
    class LOGIKA_CONNECTIONS_EXPORT IConnection;
} // namespace connections


namespace meters
{

class LOGIKA_METERS_EXPORT Meter;
class LOGIKA_METERS_EXPORT DataTag;
class LOGIKA_METERS_EXPORT ArchiveType;
class LOGIKA_METERS_EXPORT IntervalArchive;
class LOGIKA_METERS_EXPORT ServiceArchive;

} // namespace meters


namespace protocols
{

/// @brief Интерфейс класса для работы с протоколами
class LOGIKA_PROTOCOLS_EXPORT IProtocol
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

    /// @brief Закрытие сессии соединения
    /// @param[in] srcNt NT отправителя
    /// @param[in] dstNt NT получателя
    virtual void CloseCommSession( const ByteType* srcNt, const ByteType* dstNt ) = 0;

    /// @brief Ожидание таймаута
    /// @details Усыпление потока исполнения на duration миллисекунд
    /// с возможностью досрочного пробуждения (@see CancelWait)
    /// @param[in] duration Таймаут (мс)
    /// @return true - Пробуждение по истечении таймаута, false - иначе
    virtual bool WaitFor( TimeType duration ) = 0;

    /// @brief Отмена активного ожидания
    virtual void CancelWait() = 0;

    /// @brief Определение типа прибора
    /// @param[in] srcNt NT источника
    /// @param[in] dstNt NT назначения
    /// @param[out] extraData Дополнительная информация о модели прибора
    virtual std::shared_ptr< meters::Meter > GetMeterType( const ByteType* srcNt,
        const ByteType* dstNt, LocString& extraData ) = 0;

    /// @brief Обновление значений тэгов
    /// @param[in] srcNt NT источника
    /// @param[in] dstNt NT назначения
    /// @param[inout] tags Обновляемые тэги
    virtual void UpdateTags( const ByteType* srcNt, const ByteType* dstNt,
        std::vector< std::shared_ptr< meters::DataTag > >& tags ) = 0;

    /// @brief Чтение описания интервального архива
    /// @param[in] meter Прибор
    /// @param[in] srcNt NT источника
    /// @param[in] dstNt NT назначения
    /// @param[in] archiveType Тип архива
    /// @param[out] state Состояние запроса на чтение
    /// @return Описание интервального архива
    virtual std::shared_ptr< meters::IntervalArchive > ReadIntervalArchiveDef( std::shared_ptr< meters::Meter > meter,
        const ByteType* srcNt, const ByteType* dstNt, std::shared_ptr< meters::ArchiveType > archiveType,
        std::shared_ptr< logika::Any >& state ) = 0;

    /// @brief Чтение интервального архива
    /// @param[in] meter Прибор
    /// @param[in] srcNt NT источника
    /// @param[in] dstNt NT назначения
    /// @param[out] archive Считываемый архив
    /// @param[in] start Начало интервала
    /// @param[in] end Конец интервала
    /// @param[out] state Состояние запроса на чтение
    /// @param[out] progress Прогресс чтения (0-100)
    /// @return true - остались непрочитанные данные, false - иначе
    virtual bool ReadIntervalArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
        const ByteType* dstNt, std::shared_ptr< meters::IntervalArchive > archive, TimeType start, TimeType end,
        std::shared_ptr< logika::Any >& state, double& progress ) = 0;

    /// @brief Чтение сервисного архива
    /// @param[in] meter Прибор
    /// @param[in] srcNt NT источника
    /// @param[in] dstNt NT назначения
    /// @param[out] archive Считываемый архив
    /// @param[in] start Начало интервала
    /// @param[in] end Конец интервала
    /// @param[out] state Состояние запроса на чтение
    /// @param[out] progress Прогресс чтения (0-100)
    /// @return true - остались непрочитанные данные, false - иначе
    virtual bool ReadServiceArchive( std::shared_ptr< meters::Meter > meter, const ByteType* srcNt,
        const ByteType* dstNt, std::shared_ptr< meters::ServiceArchive > archive, TimeType start, TimeType end,
        std::shared_ptr< logika::Any >& state, double& progress ) = 0;

    /// @brief Получене времени прибора
    /// @param[in] meter Прибор
    /// @param[in] srcNt NT источника
    /// @param[in] dstNt NT назначения
    /// @return Время прибора
    virtual TimeType GetDeviceClock( std::shared_ptr< meters::Meter > meter,
        const ByteType* srcNt, const ByteType* dstNt ) = 0;

}; // class IProtocol

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_IPROTOCOL_H

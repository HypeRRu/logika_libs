/// @file Объявление протокола M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H
#define LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H

#include <logika/protocols/defs.h>
#include <logika/protocols/protocol.h>
#include <logika/protocols/m4/opcodes.h>

#include <logika/common/types.h>
#include <logika/connections/serial/types.h>

namespace logika
{

namespace protocols
{

namespace M4
{

namespace MeterChannel
{

using Type = ByteType;

/// @brief Номер канала
enum : Type
{
    Sys = 0,    ///< Системный канал
    Tv1 = 1,    ///< 1 канал тепловычислителя
    Tv2 = 2     ///< 2 канал тепловычислителя
}; // anonymous enum

} // namespace MeterChannel


/// @brief Протокол M4
class LOGIKA_PROTOCOLS_EXPORT M4Protocol: public Protocol
{
public:
    struct BusActiveState;

    static const ByteType FRAME_START;              ///< Начало кадра
    static const ByteType FRAME_END;                ///< Конец кадра
    static const ByteType EXT_PROTO;                ///< @todo
    static const ByteType BROADCAST_NT;             ///< NT для безадресных (широковещательных) запросов
    static const uint32_t MAX_RAM_REQUEST;          ///< Максимальный объем данных, запрашиваемых из RAM
    static const uint32_t MAX_TAGS_AT_ONCE;         ///< Максимальное количество считываемых за один запрос тэгов
    static const uint16_t PARTITION_CURRENT;        ///< Текущий раздел
    static const uint32_t ALT_SPEED_FALLBACK_TIME;  ///< Время сброса альтернативной скорости работы
    static const ByteVector WAKEUP_SEQUENCE;        ///< Последовательность байтов для "пробуждения" прибора

public:
    /// @brief Конструктор шины
    /// @param[in] targetBaudRate Скорость работы с прибором
    M4Protocol( connections::BaudRate::Type targetBaudRate = connections::BaudRate::NotSupported );

    /// @brief Получение текущей скорости работы прибора
    /// @return Текущая скорость работы прибора (если прибор Serial), иначе NotSupported
    /// @throws std::logical_error если тип соединения указан как serial,
    /// но при этом соединение не является SerialConnection
    connections::BaudRate::Type GetCurrentBaudRate() const;

    /// @copydoc Protocol::ResetBusActiveState()
    virtual void ResetBusActiveState() override;

    /// @brief "Пробуждение" прибора
    /// @param[in] slowWake Нужно ли ожидание между байтами последовательности для пробуждения
    /// @note Совсем старым приборам нужна пауза между байтами FF
    void SendAttention( bool slowWake );

protected:
    /// @copydoc Protocol::CloseCommSessionImpl()
    virtual void CloseCommSessionImpl( ByteType srcNt, ByteType dstNt ) override;

private:
    /// @brief Сброс скорости работы прибора до начального значения
    /// @throws std::logical_error если тип соединения указан как serial,
    /// но при этом соединение не является SerialConnection
    void SerialSpeedFallback();

private:
    connections::BaudRate::Type initialBaudRate_;   ///< Начальное значение BaudRate для соединений с автоподнятием скорости
    connections::BaudRate::Type suggestedBaudRate_; ///< Текущее значение BaudRate
    std::shared_ptr< BusActiveState > activeDev_;   ///< Внутреннее состояние шины

public:
    /// @brief Внутреннее состояние шины
    struct BusActiveState
    {
    public:
        /// @brief Конструктор внутреннего состояния шины
        /// @param[in] mtr Прибор
        /// @param[in] ntByte Байт nt
        /// @param[in] chan Номер канала (для Logika4L)
        BusActiveState( std::shared_ptr< meters::Meter > mtr, ByteType ntByte, MeterChannel::Type chan );

        /// @brief Получение времени (мс) с момента последнего ввода/вывода
        /// @return Время с момента последнего ввода/вывода (мс)
        TimeType GetTimeFromLastIo() const;

    public:
        const std::shared_ptr< meters::Meter > meter;   ///< Активный прибор
        const ByteType nt;                              ///< Байт NT
        const MeterChannel::Type tv;                    ///< Канал (только для Logika4L)
        TimeType lastIo;                                ///< Метка времени последнего ввода/вывода (мс)
        bool hasIoError;                                ///< Наличие ошибки ввода/вывода

    }; // struct BusActiveState

}; // struct M4Protocol

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H

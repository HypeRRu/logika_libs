/// @file Объявление структуры пакета в протоколе M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_PACKET_H
#define LOGIKA_PROTOCOLS_M4_PACKET_H

#include <logika/protocols/defs.h>
#include <logika/protocols/m4/opcodes.h>

#include <logika/common/types.h>

namespace logika
{

namespace protocols
{

namespace M4
{

/// @brief Структура пакета в протоколе M4
struct LOGIKA_PROTOCOLS_EXPORT Packet
{
public:
    /// @brief Конструктор по умолчанию
    Packet();

    /// @brief Конструктор с параметрами
    /// @param[in] ntByte Значение байта NT
    /// @param[in] isExtended Расширенный пакет
    /// @param[in] identifier Идентификатор
    /// @param[in] attributesByte Атрибуты пакета
    /// @param[in] opcode Код операции
    /// @param[in] dataVector Данные пакета
    /// @param[in] cs Контрольная сумма
    Packet( ByteType ntByte, bool isExtended, ByteType identifier, ByteType attributesByte
        , Opcode::Type opcode, const ByteVector& dataVector, uint16_t cs );

    /// @brief Формирование пакета
    /// @return Массив байтов - пакет
    ByteVector GetDump() const;

public:
    ByteType nt;                ///< Значение байта NT
    bool extended;              ///< Расширенный пакет
    ByteType id;                ///< Идентификатор
    ByteType attributes;        ///< Атрибуты пакета
    Opcode::Type funcOpcode;    ///< Код операции
    ByteVector data;            ///< Данные пакета
    uint16_t checkSum;          ///< Контрольная сумма. CRC16 для расширенного пакета и CSUM8 для "легаси"

}; // struct Packet

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_PACKET_H

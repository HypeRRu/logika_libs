/// @file Определение кодов операции при работе с протоколом M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_OPCODES_H
#define LOGIKA_PROTOCOLS_M4_OPCODES_H

#include <logika/common/types.h>

namespace logika
{

namespace protocols
{

namespace M4
{

namespace Opcode
{

using Type = ByteType;

/// @brief Коды операции при работе с протоколом M4
enum : Type
{
    NoOp                = static_cast< Type >(  0x0 ),  ///< Операция не задана
    /// "Легаси" коды операций M4
    Error               = static_cast< Type >( 0x21 ),  ///< Ошибка
    Handshake           = static_cast< Type >( 0x3F ),  ///< Рукопожатие
    SetSpeed            = static_cast< Type >( 0x42 ),  ///< Установка скорости работы
    WriteParam          = static_cast< Type >( 0x44 ),  ///< Запись параметра
    ReadFlash           = static_cast< Type >( 0x45 ),  ///< Чтение флэш-памяти
    CalcControl         = static_cast< Type >( 0x4F ),  ///< Контрольная сумма
    ReadRam             = static_cast< Type >( 0x52 ),  ///< Чтение RAM
    DeviceDiscovery     = static_cast< Type >( 0xF0 ),  ///< Автообнаружение устройств на шине (ЛГК410)       
    SessionClose        = static_cast< Type >( 0x71 ),  ///< Закрытие сессии
    /// Современные коды операций M4
    ReadArchive         = static_cast< Type >( 0x61 ),  ///< Чтение архива
    ReadTags            = static_cast< Type >( 0x72 ),  ///< Чтение тэгов
    WriteTags           = static_cast< Type >( 0x77 ),  ///< Запись тэгов
    SetTechnologicalNT  = static_cast< Type >( 0xF1 ),  ///< Установка NT для технологического режима (ЛГК410)
}; // anonymous enum

} // namespace Opcode

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_OPCODES_H

/// @file Определение кодов возврата при работе с протоколами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_RC_H
#define LOGIKA_PROTOCOLS_RC_H

#include <logika/common/rc.h>

namespace logika
{

namespace protocols
{

namespace Rc
{

using Type = logika::Rc::Type;

/// @brief Коды возврата при работе с протоколами
enum : Type
{
    Success             = logika::Rc::Success,              ///< ( 0x0;          0 ) Успешное завершение операции
    Fail                = logika::Rc::Fail,                 ///< ( 0xFFFF;   65535 ) Провал операции
    RcShift             = logika::Rc::RcShift,              ///< Сдвиг кода возврата
    ProtocolRc          = 0x2,                              ///< Индикатор кода возврата для соединений

    PacketReceived      = ( 0x1 << RcShift ) | ProtocolRc,  ///< ( 0x10002;  65538 ) Пакет успешно получен
    PacketTransmitted   = ( 0x2 << RcShift ) | ProtocolRc,  ///< ( 0x20002; 131074 ) Пакет успешно передан
    RxTimeoutError      = ( 0x3 << RcShift ) | ProtocolRc,  ///< ( 0x30002; 196610 ) Истекло время ожидания получения
    RxCrcError          = ( 0x4 << RcShift ) | ProtocolRc,  ///< ( 0x40002; 262146 ) Ошибка CRC (Cyclic Redundancy Check)
    RxLatePacketError   = ( 0x5 << RcShift ) | ProtocolRc,  ///< ( 0x50002; 327682 ) Пакет пришел не вовремя
    RxGeneralError      = ( 0x6 << RcShift ) | ProtocolRc,  ///< ( 0x60002; 393218 ) Неклассифицировання ошибка получения
    BadRequestError     = ( 0x7 << RcShift ) | ProtocolRc,  ///< ( 0x70002; 458754 ) Неверная структура запроса
    WriteProtectedError = ( 0x8 << RcShift ) | ProtocolRc,  ///< ( 0x80002; 524290 ) Запись запрещена
    InvalidArgError     = ( 0x9 << RcShift ) | ProtocolRc,  ///< ( 0x90002; 589826 ) Неверный аргумент
}; // anonymous enum

} // namespace Rc

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_RC_H

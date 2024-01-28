/// @file Определение кодов возврата при работе с соединениями
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_RC_H
#define LOGIKA_CONNECTIONS_RC_H

#include <logika/common/rc.h>

namespace logika
{

namespace connections
{

namespace Rc
{

using Type = logika::Rc::Type;

/// @brief Коды возврата при работе с соединениями
enum : Type
{
    Success                 = logika::Rc::Success,                  ///< ( 0x0;          0 ) Успешное завершение операции
    Fail                    = logika::Rc::Fail,                     ///< ( 0xFFFF;   65535 ) Провал операции
    RcShift                 = logika::Rc::RcShift,                  ///< Сдвиг кода возврата
    ConnectionRc            = 0x1,                                  ///< Индикатор кода возврата для соединений

    ConnectionNotSetError   = ( 0x1 << RcShift ) | ConnectionRc,    ///< ( 0x10001;  65537 ) Соединение не установлено
    TimeOutError            = ( 0x2 << RcShift ) | ConnectionRc,    ///< ( 0x20001; 131073 ) Время ожидания истекло
    InvalidArgError         = ( 0x3 << RcShift ) | ConnectionRc,    ///< ( 0x30001; 196609 ) Передан неверный аргумент
    PollError               = ( 0x4 << RcShift ) | ConnectionRc,    ///< ( 0x40001; 262145 ) Ошибка ожидания событий
    ReadError               = ( 0x5 << RcShift ) | ConnectionRc,    ///< ( 0x50001; 327681 ) Ошибка чтения
    WriteError              = ( 0x6 << RcShift ) | ConnectionRc,    ///< ( 0x60001; 393217 ) Ошибка записи
}; // anonymous enum

} // namespace Rc

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_RC_H

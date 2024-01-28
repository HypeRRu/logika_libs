/// @file Определение общих кодов возврата
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_RC_H
#define LOGIKA_COMMON_RC_H

#include <cstdint>

namespace logika
{

namespace Rc
{

using Type = uint32_t;

/// @brief Коды возврата
enum : Type
{
    Success = 0,        ///< ( 0x0;        0 ) Успешное завершение операции
    Fail    = 0xFFFF,   ///< ( 0xFFFF; 65535 ) Провал операции
    RcShift = 0x10,     ///< Сдвиг кода возврата
}; // anonymous enum

} // namespace Rc

} // namespace logika

#endif // LOGIKA_COMMON_RC_H

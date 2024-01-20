/// @file Объявление уровней логирования
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_LOG_TYPES_H
#define LOGIKA_LOG_TYPES_H

#include <cstdint>

namespace logika
{

namespace log
{

namespace LogLevel
{

using Type = uint8_t;

/// @brief Уровни логирования
enum : Type
{
    LogDisable = 0,
    LogError,
    LogWarning,
    LogInfo,
    LogDebug,

    LogAll,
    Count = LogAll
}; // anonymous enum

} // namespace LogLevel

namespace LogType
{

using Type = uint8_t;

/// @brief Тип логирования
enum : Type
{
    LogDisable = 0, ///< Логирование отключено
    LogConsole = 1, ///< Логирование в консоль
    LogFile    = 2  ///< Логирование в файл
}; // anonymous enum

} // namespace LogType

} // namespace log

} // namespace logika

#endif // LOGIKA_LOG_TYPES_H

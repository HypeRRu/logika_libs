/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_MISC_H
#define LOGIKA_COMMON_MISC_H

#include <logika/common/types.h>

#include <string>

namespace logika
{

/// @brief Получение строки с описанием ошибки по ее коду (errno)
/// @details Потокобезопасная версия функции strerror
/// @param[in] error Код ошибки (errno)
/// @return Описание ошибки
std::string SafeStrError( int error );

/// @brief Получение строки со временем в формате dd.MM.yy HH:mm:ss
/// @param[in] timestamp Конвертируемая метка времени. При значении 0 берется текущее время
/// @return Строка со временем
std::string GetTimeString( TimeType timestamp = 0 );

} // namespace logika

#endif // LOGIKA_COMMON_MISC_H

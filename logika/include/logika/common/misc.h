/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_MISC_H
#define LOGIKA_COMMON_MISC_H

#include <string>

namespace logika
{

/// @brief Получение строки с описанием ошибки по ее коду (errno)
/// @details Потокобезопасная версия функции strerror
/// @param[in] error Код ошибки (errno)
/// @return Описание ошибки
std::string SafeStrError( int error );

} // namespace logika

#endif // LOGIKA_COMMON_MISC_H

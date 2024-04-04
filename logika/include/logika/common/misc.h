/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_MISC_H
#define LOGIKA_COMMON_MISC_H

#include <logika/common/types.h>

/// @cond
#include <string>
/// @endcond

namespace logika
{

/// @brief Конвертация из std::string в LocString
/// @param[in] str Конвертируемая строка
/// @return Многобайтовая строка
/// @note Не используйте в многопоточной среде!
LocString ToLocString( const std::string& str );

/// @brief Заглушка ToLocString для LocString
/// @param[in] str Конвертируемая строка
/// @return Переданная строка
const LocString& ToLocString( const LocString& str );

/// @brief Получение строки с описанием ошибки по ее коду (errno)
/// @details Потокобезопасная версия функции strerror
/// @param[in] error Код ошибки (errno)
/// @return Описание ошибки
LocString SafeStrError( int error );

/// @brief Получение строки со временем в формате dd.MM.yy HH:mm:ss
/// @param[in] timestamp Конвертируемая метка времени. При значении 0 берется текущее время
/// @return Строка со временем
LocString GetTimeString( TimeType timestamp = 0 );

/// @brief Сравнение строк на равенство без учета регистра
/// @param[in] lhs Первая строка
/// @param[in] rhs Вторая строка
/// @return Равны ли строки
bool StrCaseEq( const LocString& lhs, const LocString& rhs );

/// @brief Обрезание пробельных символов из строки
/// @param[in] str Обрабатываемая строка
/// @param[in] trimChars Обрезаемые символы
/// @return Обработанная строка
LocString Trim( const LocString& str, const LocString& trimChars = L" \t\r\n" );

/// @brief Получение индексов единичных битов
/// @param[in] bits Набор битов
/// @param[in] bitsCount Количество проверяемых битов
/// @param[in] offset Номер первого проверяемого бита
/// @return Массив индексов единичных битов
std::vector< size_t > BitNumbers( uint64_t bits, size_t bitsCount, size_t offset );

/// @brief Получение индексов единичных битов
/// @param[in] buffer Набор битов
/// @param[in] bitsCount Количество проверяемых битов
/// @param[in] offset Номер первого проверяемого бита
/// @return Массив индексов единичных битов
std::vector< size_t > BitNumbers( const std::vector< ByteType >& buffer, size_t bitsCount, size_t offset );

} // namespace logika

#endif // LOGIKA_COMMON_MISC_H

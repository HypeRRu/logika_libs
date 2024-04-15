/// @file Определение общих функция для библиотеки
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_MISC_H
#define LOGIKA_COMMON_MISC_H

#include <logika/common/defs.h>
#include <logika/common/types.h>

/// @cond
#include <string>
#include <ctime>
/// @endcond

namespace logika
{

/// @brief Конвертация из std::string в LocString
/// @param[in] str Конвертируемая строка
/// @return Многобайтовая строка
/// @note Не используйте в многопоточной среде!
LOGIKA_COMMON_EXPORT LocString ToLocString( const std::string& str );

/// @brief Заглушка ToLocString для LocString
/// @param[in] str Конвертируемая строка
/// @return Переданная строка
LOGIKA_COMMON_EXPORT const LocString& ToLocString( const LocString& str );

/// @brief Получение строки с описанием ошибки по ее коду (errno)
/// @details Потокобезопасная версия функции strerror
/// @param[in] error Код ошибки (errno)
/// @return Описание ошибки
LOGIKA_COMMON_EXPORT LocString SafeStrError( int error );

/// @brief Получение текущей метки времени
/// @return Текущая метка времени (мс)
LOGIKA_COMMON_EXPORT TimeType GetCurrentTimestamp();

/// @brief Получение структуры со временем
/// @param[in] timestamp Конвертируемая метка времени (мс). При значении 0 берется текущее время
/// @param[out] millis Количество милисекунд
/// @return Структура со временем
LOGIKA_COMMON_EXPORT struct tm GetTimeStruct( TimeType timestamp = 0, uint16_t* millis = nullptr );

/// @brief Получение структуры со временем из строки
/// @param[in] time Конвертируемое время
/// @param[in] format Формат времени
/// @param[out] timeStruct Полученное время
/// @return Удалось ли конвертировать
LOGIKA_COMMON_EXPORT bool GetTimeFromString( const LocString& time,
    const std::string& format, struct tm& timeStruct );

/// @brief Получение строки со временем в формате dd.MM.yy HH:mm:ss
/// @param[in] timestamp Конвертируемая метка времени (мс). При значении 0 берется текущее время
/// @return Строка со временем
LOGIKA_COMMON_EXPORT LocString GetTimeString( TimeType timestamp = 0 );

/// @brief Получение строки со временем заданного формата
/// @param[in] timeStruct Структура со временем
/// @param[in] format Формат времени
/// @param[in] bufSize Максимальный размер буфера для строки
/// @return Строка со временем
LOGIKA_COMMON_EXPORT LocString GetFormatTime( const struct tm& timeStruct,
    const std::string& format, size_t bufSize );

/// @brief Сравнение строк на равенство без учета регистра
/// @param[in] lhs Первая строка
/// @param[in] rhs Вторая строка
/// @return Равны ли строки
LOGIKA_COMMON_EXPORT bool StrCaseEq( const LocString& lhs, const LocString& rhs );

/// @brief Обрезание пробельных символов из строки
/// @param[in] str Обрабатываемая строка
/// @param[in] trimChars Обрезаемые символы
/// @return Обработанная строка
LOGIKA_COMMON_EXPORT LocString Trim( const LocString& str, const LocString& trimChars = LOCALIZED( " \t\r\n" ) );

/// @brief Получение индексов единичных битов
/// @param[in] bits Набор битов
/// @param[in] bitsCount Количество проверяемых битов
/// @param[in] offset Номер первого проверяемого бита
/// @return Массив индексов единичных битов
LOGIKA_COMMON_EXPORT std::vector< size_t > BitNumbers( uint64_t bits, size_t bitsCount, size_t offset );

/// @brief Получение индексов единичных битов
/// @param[in] buffer Набор битов
/// @param[in] bitsCount Количество проверяемых битов
/// @param[in] offset Номер первого проверяемого бита
/// @return Массив индексов единичных битов
LOGIKA_COMMON_EXPORT std::vector< size_t > BitNumbers(
    const std::vector< ByteType >& buffer, size_t bitsCount, size_t offset );

} // namespace logika

#endif // LOGIKA_COMMON_MISC_H

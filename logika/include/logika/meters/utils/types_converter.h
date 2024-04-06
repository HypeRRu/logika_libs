/// @file Объявление функций для конвертации типов приборов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_UTILS_TYPES_CONVERTER_H
#define LOGIKA_METERS_UTILS_TYPES_CONVERTER_H

#include <logika/meters/defs.h>
#include <logika/meters/types.h>

/// @cond
#include <string>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Конвертация типа данных элемента в БД в строку
/// @param[in] type Тип данных
/// @return Строковое представление типа данных
LOGIKA_METERS_EXPORT LocString DbTypeToString( DbType type );

/// @brief Конвертация группы тэга в строку
/// @param[in] Группа тэга
/// @return Строковое представление группы тэга
LOGIKA_METERS_EXPORT LocString TagKindToString( TagKind::Type type );

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_UTILS_TYPES_CONVERTER_H
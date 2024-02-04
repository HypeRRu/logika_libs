/// @file Объявление функций для конвертации типов приборов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_UTILS_TYPES_CONVERTER_H
#define LOGIKA_METERS_UTILS_TYPES_CONVERTER_H

#include <logika/meters/types.h>

#include <string>

namespace logika
{

namespace meters
{

/// @brief Конвертация типа данных элемента в БД в строку
/// @param[in] type Тип данных
/// @return Строковое представление типа данных
std::string DbTypeToString( DbType type );

/// @brief Конвертация группы тэга в строку
/// @param[in] Группа тэга
/// @return Строковое представление группы тэга
std::string TagKindToString( TagKind::Type type );

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_UTILS_TYPES_CONVERTER_H
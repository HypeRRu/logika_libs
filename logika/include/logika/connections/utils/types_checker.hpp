/// @file Объявление функций для проверки типов соединений
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_UTILS_TYPES_CHECKER_HPP
#define LOGIKA_CONNECTIONS_UTILS_TYPES_CHECKER_HPP

#include <logika/connections/types.h>
#include <logika/connections/serial/types.h>

namespace logika
{

namespace connections
{

/// @brief Проверка валидности типа соединения
/// @param[in] type Тип соединения
/// @return Валидность типа соединения
inline bool IsValidConnectionType( ConnectionType::Type type );

/// @brief Проверка валидности флагов очистки буфера
/// @param[in] type Флаги очистки буфера
/// @return Валидность флагов очистки буфера
inline bool IsValidPurgeFlags( PurgeFlags::Type flags );

/// @brief Проверка валидности скорости работы порта
/// @param[in] rate Скорость работы порта
/// @return Валидность скорости работы порта
inline bool IsValidBaudRate( BaudRate::Type rate );

/// @brief Проверка валидности количества стоп-битов
/// @param[in] bits Количество стоп-битов
/// @return Валидность количества стоп-битов
inline bool IsValidStopBits( StopBits::Type bits );

/// @brief Проверка валидности количества битов данных
/// @param[in] bits Количество битов данных
/// @return Валидность количества битов данных
inline bool IsValidDataBits( DataBits::Type bits );

/// @brief Проверка валидности схемы четности
/// @param[in] parity Схема четности
/// @return Валидность схемы четности
inline bool IsValidParity( Parity::Type parity );

} // namespace connections

} // namespace logika

#include <logika/connections/utils/types_checker.inl>

#endif // LOGIKA_CONNECTIONS_UTILS_TYPES_CHECKER_HPP

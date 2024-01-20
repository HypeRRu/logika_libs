/// @file Объявление функций для конвертации типов соединений
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_UTILS_TYPES_CONVERTER_H
#define LOGIKA_CONNECTIONS_UTILS_TYPES_CONVERTER_H

#include <logika/connections/types.h>
#include <logika/connections/serial/types.h>

#include <string>

namespace logika
{

namespace connections
{

/// @brief Конвертация типа соединения в строку
/// @param[in] type Тип соединения
/// @return Строковое представление типа соединения
std::string ConnectionTypeToString( ConnectionType::Type type );

/// @brief Конвертация состояния соединения в строку
/// @param[in] state Состояние соединения
/// @return Строковое представление состояния соединения
std::string ConnectionStateToString( ConnectionState state );

/// @brief Конвертация флагов очистки буфера
/// @param[in] flags Флаги очистки буфера
/// @return Строковое представление флагов очистки буфера
std::string PurgeFlagsToString( PurgeFlags::Type flags );

/// @brief Конвертация скорости работы порта в строку
/// @param[in] rate Скорость работы порта
/// @return Строковое представление скорости работы порта
std::string BaudRateToString( BaudRate::Type rate );

/// @brief Конвертация количества стоп-битов в строку
/// @param[in] bits Количество стоп-битов
/// @return Строковое представление количества стоп-битов
std::string StopBitsToString( StopBits::Type bits );

/// @brief Конвертация количества битов данных в строку
/// @param[in] bits Количество битов данных
/// @return Строковое представление количества битов данных
std::string DataBitsToString( DataBits::Type bits );

/// @brief Конвертация схемы четности в строку
/// @param[in] bits Схема четности
/// @return Строковое представление схемы четности
std::string ParityToString( Parity::Type parity );

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_UTILS_TYPES_CONVERTER_H

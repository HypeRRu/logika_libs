/// @file Объявление функций для работы с вводом/выводом через сокеты в Windows
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_COMMON_WINDOWS_SOCKET_IO_H
#define LOGIKA_CONNECTIONS_COMMON_WINDOWS_SOCKET_IO_H

#if !defined( _WIN32 ) && !defined( _WIN64 )
#error Wrong platform
#endif

#include <logika/common/types.h>

#include <logika/connections/defs.h>
#include <logika/connections/rc.h>

/// @cond
#include <functional>
/// @endcond

namespace logika
{

namespace connections
{

namespace windows_io
{

/// @brief Чтение из дескриптора в буфер
/// @param[in] sock Сокет
/// @param[out] buffer Буфер для чтения
/// @param[in] start Начиная с какого байта записывать в буфер
/// @param[in] needed Желаемое количество байтов
/// @param[in] timeout Таймаут на чтение
/// @param[out] rc Статус операции. Rc::Success в случае успеха.
/// @return Количество прочитанных байтов
/// @note После первого чтения возвращает количество прочитанных байтов
LOGIKA_CONNECTIONS_EXPORT uint32_t ReadBuffer( SocketType sock, ByteVector& buffer
    , uint32_t start, uint32_t needed, TimeType timeout = 0, Rc::Type* rc = nullptr );

/// @brief Запись в дескриптор содержимого буфера
/// @param[in] sock Сокет
/// @param[in] buffer Буфер, содержащий данные для записи
/// @param[in] start Начиная с какого байта считывать буфер
/// @param[out] rc Статус операции. Rc::Success в случае успеха.
/// @return Количество записанных байтов
/// @note После первой записи возвращает количество записанных байтов
LOGIKA_CONNECTIONS_EXPORT uint32_t WriteBuffer( SocketType sock,
    const ByteVector& buffer, uint32_t start, Rc::Type* rc = nullptr );

/// @brief Получение количества доступных для чтения байтов
/// @param[in] sock Сокет
/// @return Количество доступных для чтения байтов или -1 в случае ошибки
/// @note Не применимо к UDP-сокетам
LOGIKA_CONNECTIONS_EXPORT int32_t BytesAvailable( SocketType sock );

} // namespace windows_io

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_COMMON_WINDOWS_SOCKET_IO_H

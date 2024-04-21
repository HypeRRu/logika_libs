/// @file Объявление функций для работы с вводом/выводом в Linux
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_COMMON_LINUX_IO_H
#define LOGIKA_CONNECTIONS_COMMON_LINUX_IO_H

#if !defined( __linux__ ) && !defined( __APPLE__ )
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

namespace linux_io
{

using ReadFunction = std::function< ssize_t( FileHandleType, void*, size_t ) >;
using WriteFunction = std::function< ssize_t( FileHandleType, const void*, size_t ) >;

/// @brief Чтение из дескриптора в буфер
/// @param[in] readfn Функция чтения
/// @param[in] handle Дескриптор файла
/// @param[out] buffer Буфер для чтения
/// @param[in] start Начиная с какого байта записывать в буфер
/// @param[in] needed Желаемое количество байтов
/// @param[in] timeout Таймаут на чтение
/// @param[out] rc Статус операции. Rc::Success в случае успеха.
/// @return Количество прочитанных байтов
/// @note После первого чтения возвращает количество прочитанных байтов
LOGIKA_CONNECTIONS_EXPORT uint32_t ReadBuffer( ReadFunction readfn, FileHandleType handle, ByteVector& buffer
    , uint32_t start, uint32_t needed, TimeType timeout = 0, Rc::Type* rc = nullptr );

/// @brief Запись в дескриптор содержимого буфера
/// @param[in] writefn Функция записи
/// @param[in] handle Дескриптор файла
/// @param[in] buffer Буфер, содержащий данные для записи
/// @param[in] start Начиная с какого байта считывать буфер
/// @param[out] rc Статус операции. Rc::Success в случае успеха.
/// @return Количество записанных байтов
/// @note После первой записи возвращает количество записанных байтов
LOGIKA_CONNECTIONS_EXPORT uint32_t WriteBuffer( WriteFunction writefn, FileHandleType handle
    , const ByteVector& buffer, uint32_t start, Rc::Type* rc = nullptr );

/// @brief Получение количества доступных для чтения байтов
/// @param[in] handle Дескриптор файла
/// @return Количество доступных для чтения байтов или -1 в случае ошибки
/// @note Не применимо к UDP-сокетам
LOGIKA_CONNECTIONS_EXPORT int32_t BytesAvailable( FileHandleType handle );

} // namespace linux_io

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_COMMON_LINUX_IO_H

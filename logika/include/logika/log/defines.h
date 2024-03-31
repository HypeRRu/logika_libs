/// @file Объявление макросов для использования логгера
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_LOG_DEFINES_H
#define LOGIKA_LOG_DEFINES_H

#include <logika/log/logger.h>

/// @cond
#include <sstream>
/// @endcond

namespace logika
{

/// @brief Уровни логирования
#define LOG_ERROR logika::log::LogLevel::LogError
#define LOG_WARNING logika::log::LogLevel::LogWarning
#define LOG_INFO logika::log::LogLevel::LogInfo
#define LOG_DEBUG logika::log::LogLevel::LogDebug

/// @brief Запись в лог с использованием потока
#define LOG_WRITE( level, message ) \
do { \
    logika::LocStringStream logstr; \
    logstr << message; \
    logika::log::Logger::Instance().Write( level, logstr.str(), __FUNCTION__ ); \
} while ( false );

/// @brief Запись в лог заданного сообщения
#define LOG_WRITE_MSG( level, message ) \
do { \
    logika::log::Logger::Instance().Write( level, message, __FUNCTION__ ); \
} while ( false );

} // namespace logika

#endif // LOGIKA_LOG_DEFINES_H

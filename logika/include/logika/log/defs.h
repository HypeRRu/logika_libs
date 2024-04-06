/// @file Объявления библиотеки logger
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_LOG_DEFS_H
#define LOGIKA_LOG_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_logger_EXPORT )
#       define LOGIKA_LOGGER_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_LOGGER_EXPORT __declspec( dllimport )
#   endif // defined( logika_logger_EXPORT )
#else
#   define LOGIKA_LOGGER_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_LOG_DEFS_H

/// @file Объявления библиотеки common
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_DEFS_H
#define LOGIKA_COMMON_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_common_EXPORT )
#       define LOGIKA_COMMON_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_COMMON_EXPORT __declspec( dllimport )
#   endif // defined( logika_common_EXPORT )
#else
#   define LOGIKA_COMMON_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_COMMON_DEFS_H

/// @file Объявления библиотеки protocols
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_DEFS_H
#define LOGIKA_PROTOCOLS_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_protocols_EXPORT )
#       define LOGIKA_PROTOCOLS_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_PROTOCOLS_EXPORT __declspec( dllimport )
#   endif // defined( logika_protocols_EXPORT )
#else
#   define LOGIKA_PROTOCOLS_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_PROTOCOLS_DEFS_H

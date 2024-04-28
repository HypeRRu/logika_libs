/// @file Объявления библиотеки meters6
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA6_DEFS_H
#define LOGIKA_METERS_LOGIKA6_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_meters6_EXPORT )
#       define LOGIKA_METERS_6_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_METERS_6_EXPORT __declspec( dllimport )
#   endif // defined( logika_meters4_EXPORT )
#else
#   define LOGIKA_METERS_6_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_METERS_6_DEFS_H

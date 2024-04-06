/// @file Объявления библиотеки meters
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DEFS_H
#define LOGIKA_METERS_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_meters_EXPORT )
#       define LOGIKA_METERS_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_METERS_EXPORT __declspec( dllimport )
#   endif // defined( logika_meters_EXPORT )
#else
#   define LOGIKA_METERS_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_METERS_DEFS_H

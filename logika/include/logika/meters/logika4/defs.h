/// @file Объявления библиотеки meters4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_DEFS_H
#define LOGIKA_METERS_LOGIKA4_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_meters4_EXPORT )
#       define LOGIKA_METERS_4_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_METERS_4_EXPORT __declspec( dllimport )
#   endif // defined( logika_meters4_EXPORT )
#else
#   define LOGIKA_METERS_4_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_METERS_4_DEFS_H

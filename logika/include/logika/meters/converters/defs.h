/// @file Объявления библиотеки resource_converters
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CONVERTERS_DEFS_H
#define LOGIKA_METERS_CONVERTERS_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_resource_converters_EXPORT )
#       define LOGIKA_RESOURCE_CONVERTERS_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_RESOURCE_CONVERTERS_EXPORT __declspec( dllimport )
#   endif // defined( logika_resource_converters_EXPORT )
#else
#   define LOGIKA_RESOURCE_CONVERTERS_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_METERS_CONVERTERS_DEFS_H

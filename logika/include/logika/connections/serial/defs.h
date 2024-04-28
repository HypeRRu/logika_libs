/// @file Объявления библиотеки connections_serial
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_SERIAL_DEFS_H
#define LOGIKA_CONNECTIONS_SERIAL_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_connections_serial_EXPORT )
#       define LOGIKA_CONNECTIONS_SERIAL_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_CONNECTIONS_SERIAL_EXPORT __declspec( dllimport )
#   endif // defined( logika_connections_SERIAL_EXPORT )
#else
#   define LOGIKA_CONNECTIONS_SERIAL_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_CONNECTIONS_SERIAL_DEFS_H

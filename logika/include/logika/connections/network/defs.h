/// @file Объявления библиотеки connections_network
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_NETWORK_DEFS_H
#define LOGIKA_CONNECTIONS_NETWORK_DEFS_H

#if defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )
#   if defined( logika_connections_network_EXPORT )
#       define LOGIKA_CONNECTIONS_NETWORK_EXPORT __declspec( dllexport )
#   else
#       define LOGIKA_CONNECTIONS_NETWORK_EXPORT __declspec( dllimport )
#   endif // defined( logika_connections_SERIAL_EXPORT )
#else
#   define LOGIKA_CONNECTIONS_NETWORK_EXPORT
#endif // defined ( WIN32 ) && defined ( BUILD_SHARED_LIBS )

#endif // LOGIKA_CONNECTIONS_NETWORK_DEFS_H

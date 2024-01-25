/// @file Определение общих для библиотеки типов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_TYPES_H
#define LOGIKA_COMMON_TYPES_H

#include <vector>

#include <cstdint>

#if defined( __linux__ ) || defined( __APPLE__ )
#include <netdb.h>      /// Для struct addrinfo
#endif // defined( __linux__ ) || defined( __APPLE__ )

#if defined( _WIN32 ) || defined( _WIN64 )
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>      /// Для HANDLE
#include <winsock2.h>	/// Для SOCKET и ADDRINFOA
#endif // defined( _WIN32 ) || defined( _WIN64 )

namespace logika
{

using TimeType = uint64_t;
using ByteType = char;
using ByteVector = std::vector< ByteType >;

#if defined( __linux__ ) || defined( __APPLE__ )
using FileHandleType = int;
#define LOGIKA_FILE_HANDLE_INVALID -1
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using FileHandleType = HANDLE;
#define LOGIKA_FILE_HANDLE_INVALID INVALID_HANDLE_VALUE
#endif // defined( _WIN32 ) || defined( _WIN64 )

#if defined( __linux__ ) || defined( __APPLE__ )
using SocketType = int;
#define LOGIKA_SOCKET_INVALID -1
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using SocketType = SOCKET;
#define LOGIKA_SOCKET_INVALID INVALID_SOCKET
#endif // defined( _WIN32 ) || defined( _WIN64 )

#if defined( __linux__ ) || defined( __APPLE__ )
using NetworkAddressInfo = struct addrinfo;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using NetworkAddressInfo = ADDRINFOA;
#endif // defined( _WIN32 ) || defined( _WIN64 )

} // namespace logika

#endif // LOGIKA_COMMON_TYPES_H

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
#include <winnt.h>      /// Для HANDLE
#include <ws2tcpip.h>   /// Для struct addrinfo
#endif // defined( _WIN32 ) || defined( _WIN64 )

namespace logika
{

using TimeType = uint64_t;
using ByteType = uint8_t;
using ByteVector = std::vector< ByteType >;

#if defined( __linux__ ) || defined( __APPLE__ )
using FileHandleType = int;
constexpr FileHandleType handleInvalid = -1;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using FileHandleType = HANDLE;
constexpr FileHandleType handleInvalid = INVALID_HANDLE_VALUE;
#endif // defined( _WIN32 ) || defined( _WIN64 )

#if defined( __linux__ ) || defined( __APPLE__ )
using SocketType = int;
constexpr SocketType socketInvalid = -1;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using SocketType = SOCKET;
constexpr SocketType socketInvalid = INVALID_SOCKET;
#endif // defined( _WIN32 ) || defined( _WIN64 )

using NetworkAddressInfo = struct addrinfo;

} // namespace logika

#endif // LOGIKA_COMMON_TYPES_H

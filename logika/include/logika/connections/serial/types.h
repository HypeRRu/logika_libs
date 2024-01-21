/// @file Определение типов для последовательных соединений
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_CONNECTIONS_SERIAL_TYPES_H
#define LOGIKA_CONNECTIONS_SERIAL_TYPES_H

#include <cstdint>

#if defined( __linux__ ) || defined( __APPLE__ )
#include <termios.h>
#elif defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#else
#error Platform not supported
#endif

namespace logika
{

namespace connections
{

namespace BaudRate
{

#if defined( __linux__ ) || defined( __APPLE__ )
using Type = speed_t;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using Type = DWORD;
#endif // defined( _WIN32 ) || defined( _WIN64 )

/// @brief Скорость работы бит/с
/// @var NotSupported Неподдерживаемая скорость работы
/// @var Rate110 Скорость 110 бит/c
/// @var Rate300 Скорость 300 бит/c
/// @var Rate600 Скорость 600 бит/c
/// @var Rate1200 Скорость 1200 бит/c
/// @var Rate2400 Скорость 2400 бит/c
/// @var Rate4800 Скорость 4800 бит/c
/// @var Rate9600 Скорость 9600 бит/c
/// @var Rate19200 Скорость 19200 бит/c
/// @var Rate38400 Скорость 38400 бит/c
/// @var Rate57600 Скорость 57600 бит/c
/// @var Rate115200 Скорость 115200 бит/c
/// @var RateDefault Скорость по умолчанию. Равна 2400 бит/c
/// @todo Добавить специфические для Windows и Linux скорости
enum : Type
{
    NotSupported    = static_cast< Type >( -1 ),
#if defined( __linux__ ) || defined( __APPLE__ )
    Rate110         = B110,
    Rate300         = B300,
    Rate600         = B600,
    Rate1200        = B1200,
    Rate2400        = B2400,
    Rate4800        = B4800,
    Rate9600        = B9600,
    Rate19200       = B19200,
    Rate38400       = B38400,
    Rate57600       = B57600,
    Rate115200      = B115200,
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
    Rate110         = CBR_110,
    Rate300         = CBR_300,
    Rate600         = CBR_600,
    Rate1200        = CBR_1200,
    Rate2400        = CBR_2400,
    Rate4800        = CBR_4800,
    Rate9600        = CBR_9600,
    Rate19200       = CBR_19200,
    Rate38400       = CBR_38400,
    Rate57600       = CBR_57600,
    Rate115200      = CBR_115200,
#endif // defined( _WIN32 ) || defined( _WIN64 )
    RateDefault     = Rate2400
}; // anonymous enum

} // namespace BaudRate

namespace StopBits
{

#if defined( __linux__ ) || defined( __APPLE__ )
using Type = tcflag_t;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using Type = BYTE;
#endif // defined( _WIN32 ) || defined( _WIN64 )

/// @brief Количество используемых стоп-битов
/// @var NotSupported Неподдерживаемое количество стоп-битов
/// @var StopBitOne 1 стоп-бит
/// @var StopBitOneAndHalf 1.5 стоп-бита
/// @var StopBitsTwo 2 стоп-бита
enum : Type
{
    NotSupported = static_cast< Type >( -1 ),
#if defined( __linux__ ) || defined( __APPLE__ )
    StopBitOne          = 0,
    StopBitOneAndHalf   = NotSupported,
    StopBitsTwo         = CSTOPB
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
    StopBitOne          = ONESTOPBIT,
    StopBitOneAndHalf   = ONE5STOPBITS,
    StopBitsTwo         = TWOSTOPBITS
#endif // defined( _WIN32 ) || defined( _WIN64 )
}; // anonymous enum

} // namespace StopBits

namespace DataBits
{

#if defined( __linux__ ) || defined( __APPLE__ )
using Type = tcflag_t;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using Type = BYTE;
#endif // defined( _WIN32 ) || defined( _WIN64 )

/// @brief Количество битов данных в передаваемых и получаемых байтах
/// @var NotSupported Неподдерживаемое количество стоп-битов
/// @var DataBits5 5 битов данных в байте
/// @var DataBits6 6 битов данных в байте
/// @var DataBits7 7 битов данных в байте
/// @var DataBits8 8 битов данных в байте
enum : Type
{
    NotSupported = static_cast< Type >( -1 ),
#if defined( __linux__ ) || defined( __APPLE__ )
    DataBits5   = CS5,
    DataBits6   = CS6,
    DataBits7   = CS7,
    DataBits8   = CS8
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
    DataBits5   = 5,
    DataBits6   = 6,
    DataBits7   = 7,
    DataBits8   = 8
#endif // defined( _WIN32 ) || defined( _WIN64 )
}; // anonymous enum

} // namespace DataBits

namespace Parity
{

#if defined( __linux__ ) || defined( __APPLE__ )
using Type = tcflag_t;
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
using Type = BYTE;
#endif // defined( _WIN32 ) || defined( _WIN64 )

/// @brief Используемая схема четности
/// @var NotSupported Неподдерживаемая схема четности
/// @var ParityNone Схема четности не используется
/// @var ParityEven Ожидается четное количество единичных битов
/// @var ParityOdd Ожидается нечетное количество единичных битов
/// @var ParityMark Бит четности всегда равен 1
/// @var ParitySpace Бит четности всегда равен 0
enum : Type
{
    NotSupported = static_cast< Type >( -1 ),
#if defined( __linux__ ) || defined( __APPLE__ )
    ParityNone  = 0,
    ParityEven  = PARENB,
    ParityOdd   = PARENB | PARODD,
    ParityMark  = NotSupported,
    ParitySpace = NotSupported
#endif // defined( __linux__ ) || defined( __APPLE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
    ParityNone  = NOPARITY,
    ParityEven  = EVENPARITY,
    ParityOdd   = ODDPARITY,
    ParityMark  = MARKPARITY,
    ParitySpace = SPACEPARITY
#endif // defined( _WIN32 ) || defined( _WIN64 )
}; // anonymous enum

} // namespace Parity

} // namespace connections

} // namespace logika

#endif // LOGIKA_CONNECTIONS_SERIAL_TYPES_H

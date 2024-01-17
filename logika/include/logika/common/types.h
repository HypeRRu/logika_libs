/// @file Определение общих для библиотеки типов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_COMMON_TYPES_H
#define LOGIKA_COMMON_TYPES_H

#include <vector>

#include <cstdint>

namespace logika
{

using TimeType = uint64_t;
using ByteType = uint8_t;
using ByteVector = std::vector< ByteType >;

} // namespace logika

#endif // LOGIKA_COMMON_TYPES_H

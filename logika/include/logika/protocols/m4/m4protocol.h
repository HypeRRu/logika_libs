/// @file Объявление протокола M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H
#define LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H

#include <logika/protocols/defs.h>
#include <logika/protocols/m4/opcodes.h>

#include <logika/common/types.h>

namespace logika
{

namespace protocols
{

namespace M4
{

/// @brief Протокол M4
class LOGIKA_PROTOCOLS_EXPORT M4Protocol
{
public:
    static const ByteType FRAME_START;
    static const ByteType FRAME_END;
    static const ByteType EXT_PROTO;

}; // struct M4Protocol

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H

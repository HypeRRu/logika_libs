/// @file Реализация протокола M4
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/m4protocol.h>

namespace logika
{

namespace protocols
{

namespace M4
{

const ByteType M4Protocol::FRAME_START  = 0x10;
const ByteType M4Protocol::FRAME_END    = 0x16;
const ByteType M4Protocol::EXT_PROTO    = 0x90;

} // namespace M4

} // namespace protocols

} // namespace logika

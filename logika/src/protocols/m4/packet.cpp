/// @file Реализация структуры пакета в протоколе M4
/// @copyright HypeRRu 2024

#include <logika/protocols/m4/packet.h>

#include <logika/protocols/m4/m4protocol.h>

namespace logika
{

namespace protocols
{

namespace M4
{

Packet::Packet()
    : Packet( 0xFF, false, 0x0, 0x0, Opcode::NoOp, {}, 0x0 )
{} // Packet


Packet::Packet( ByteType ntByte, bool isExtended, ByteType identifier, ByteType attributesByte
    , Opcode::Type opcode, const ByteVector& dataVector, uint16_t cs )
    : nt{ ntByte }
    , extended{ isExtended }
    , id{ identifier }
    , attributes{ attributesByte }
    , funcOpcode{ opcode }
    , data{ dataVector }
    , checkSum{ cs }
{} // Packet( ... )


ByteVector Packet::GetDump() const
{
    ByteVector packet;

    packet.push_back( M4Protocol::FRAME_START );
    packet.push_back( nt );

    if ( extended )
    {
        packet.push_back( M4Protocol::EXT_PROTO );
        packet.push_back( id );
        packet.push_back( attributes );
        uint16_t payloadLength = static_cast< uint16_t >( data.size() + 1 );
        packet.push_back( static_cast< ByteType >( payloadLength & 0xFF ) );
        packet.push_back( static_cast< ByteType >( payloadLength >> 8 ) );
    }

    packet.push_back( static_cast< ByteType >( funcOpcode ) );
    for ( const ByteType b: data )
    {
        packet.push_back( b );
    }

    if ( extended )
    {
        packet.push_back( static_cast< ByteType >( checkSum >> 8 ) );
        packet.push_back( static_cast< ByteType >( checkSum & 0xFF ) );
    }
    else
    {
        packet.push_back( static_cast< ByteType >( checkSum & 0xFF ) );
        packet.push_back( M4Protocol::FRAME_END );
    }

    return packet;
} // GetDump

} // namespace M4

} // namespace protocols

} // namespace logika

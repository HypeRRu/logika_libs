/// @file Реализация типа канала
/// @copyright HypeRRu 2024

#include <logika/meters/channel.h>

#include <logika/meters/meter.h>
#include <logika/common/misc.h>

namespace logika
{

namespace meters
{

/// Структура описание канала

ChannelDef::ChannelDef( std::shared_ptr< Meter > m, const LocString& p, int32_t s
    , uint32_t c, const LocString& d )
    : meter{ m }
    , kind{ m ? m->GetChannelKind( s, c, p ) : ChannelKind::Undefined }
    , prefix{ p }
    , start{ s }
    , count{ c }
    , description{ d }
{} // ChannelDef


LocString ChannelDef::ToString() const
{
    return prefix + LOCALIZED( "(" ) + description + LOCALIZED( ")" );
} // ToString


/// Структура канала

Channel::Channel( const ChannelDef& cdef, int32_t channelNo )
    : ChannelDef( cdef )
    , no{ channelNo }
    , name{ cdef.prefix + ToLocString( channelNo > 0 ? std::to_string( channelNo ) : "" ) }
{} // Channel


LocString Channel::ToString() const
{
    return name + LOCALIZED( "(" ) + description + LOCALIZED( ")" );
} // ToString

} // namespace meters

} // namespace logika

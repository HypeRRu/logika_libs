/// @file Реализация типа канала
/// @copyright HypeRRu 2024

#include <logika/meters/channel.h>

namespace logika
{

namespace meters
{

/// Структура описание канала

ChannelDef::ChannelDef( std::shared_ptr< Meter > m, const std::string& p, int32_t s
    , uint32_t c, const std::string& d )
    : meter{ m }
    , kind{ m->GetChannelKind() }
    , prefix{ p }
    , start{ s }
    , count{ c }
    , description{ d }
{} // ChannelDef


std::string ChannelDef::ToString() const
{
    return prefix + "(" + description + ")";
} // ToString


/// Структура канала

Channel::Channel( const ChannelDef& cdef, int32_t channelNo )
    : ChannelDef( cdef )
    , no{ channelNo }
    , name{ cdef.prefix + ( channelNo > 0 ? std::to_string( channelNo ) : "" ) }
{} // Channel


std::string Channel::ToString() const
{
    return name + "(" + description + ")";
} // ToString

} // namespace meters

} // namespace logika

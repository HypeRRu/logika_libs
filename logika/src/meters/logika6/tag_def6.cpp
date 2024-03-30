/// @file Реализация типа тэга Logika6
/// @copyright HypeRRu 2024

#include <logika/meters/logika6/tag_def6.h>

/// @cond
#include <sstream>
#include <iomanip>
/// @endcond

namespace logika
{

namespace meters
{

DataTagDef6::DataTagDef6( const ChannelDef& cdef, const DataTagDef6Settings& settings )
    : DataTagDef( cdef, settings )
    , nodeType_{ settings.nodeType }
    , count_{ settings.count }
    , index_{ settings.index }
    , address_{ "" }
{
    std::stringstream ss;
    ss << std::setw( 3 ) << std::setfill( '0' ) << ordinal_;
    if ( Tag6NodeType::Tag != nodeType_ && 0 != index_ )
    {
        ss << "#" << std::setw( 2 ) << std::setfill( '0' ) << index_;
    }
    key_ = address_ = ss.str();
} // DataTagDef6


Tag6NodeType DataTagDef6::GetNodeType() const
{
    return nodeType_;
} // GetNodeType


uint32_t DataTagDef6::GetCount() const
{
    return count_;
} // GetCount


uint32_t DataTagDef6::GetIndex() const
{
    return index_;
} // GetIndex


std::string DataTagDef6::GetAddress() const
{
    return address_;
} // GetAddress


std::string DataTagDef6::ToString() const
{
    if ( Tag6NodeType::Array == nodeType_ )
    {
        return "array " + name_ + " " + description_;
    }
    else if ( Tag6NodeType::Structure == nodeType_ )
    {
        return "structure " + name_ + " " + description_;
    }
    else
    {
        return DataTagDef::ToString();
    }
} // ToString

} // namespace meters

} // namespace logika

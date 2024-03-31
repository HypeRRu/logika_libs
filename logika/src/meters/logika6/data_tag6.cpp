/// @file Реализация контейнера тэгов с данными Logika6
/// @copyright HypeRRu 2024

#include <logika/meters/logika6/data_tag6.h>

/// @cond
#include <sstream>
#include <iomanip>
/// @endcond

namespace logika
{

namespace meters
{

DataTag6Container::DataTag6Container( const DataTagDef6& def, int32_t channelNo
    , const std::vector< DataTagDef6 >& leafs )
    : Tag( def, channelNo )
    , dataTagDef_{ def }
    , tags_{}
{
    tags_.reserve( leafs.size() );
    for ( const DataTagDef6& leafDef: leafs )
    {
        tags_.emplace_back( leafDef, channelNo );
    }
    address_ = dataTagDef_.GetAddress();
} // DataTag6Container


const std::vector< DataTag >& DataTag6Container::GetTags() const
{
    return tags_;
} // GetTags() const


std::vector< DataTag >& DataTag6Container::GetTags()
{
    return tags_;
} // GetTags()


std::string DataTag6Container::ToString() const
{
    std::string containerType = "<U>";
    switch ( dataTagDef_.GetNodeType() )
    {
        case Tag6NodeType::Tag:
            containerType = "<T>";
            break;
        case Tag6NodeType::Array:
            containerType = "<A>";
            break;
        case Tag6NodeType::Structure:
            containerType = "<S>";
            break;
        default:
            break;
    }
    std::stringstream ordinalStr;
    ordinalStr << std::setw( 3 ) << std::setfill( '0' ) << dataTagDef_.GetOrdinal();
    return containerType + channel_.name + "." + ordinalStr.str()
        + " (" + dataTagDef_.GetDescription() + ")";
} // ToString

} // namespace meters

} // namespace logika

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

DataTag6Container::DataTag6Container( std::shared_ptr< DataTagDef6 > def, int32_t channelNo
    , const std::vector< std::shared_ptr< DataTagDef6 > >& leafs )
    : Tag( def, channelNo )
    , dataTagDef_{ def }
    , tags_{}
{
    tags_.reserve( leafs.size() );
    for ( auto leafDef: leafs )
    {
        tags_.emplace_back( std::make_shared< DataTag >( leafDef, channelNo ) );
    }
    address_ = dataTagDef_ ? dataTagDef_->GetAddress() : LOCALIZED( "" );
} // DataTag6Container


const std::vector< std::shared_ptr< DataTag > >& DataTag6Container::GetTags() const
{
    return tags_;
} // GetTags() const


std::vector< std::shared_ptr< DataTag > >& DataTag6Container::GetTags()
{
    return tags_;
} // GetTags()


LocString DataTag6Container::ToString() const
{
    LocString containerType = LOCALIZED( "<U>" );
    auto nodeType = dataTagDef_ ? dataTagDef_->GetNodeType() : Tag6NodeType::Tag;
    switch ( nodeType )
    {
        case Tag6NodeType::Tag:
            containerType = LOCALIZED( "<T>" );
            break;
        case Tag6NodeType::Array:
            containerType = LOCALIZED( "<A>" );
            break;
        case Tag6NodeType::Structure:
            containerType = LOCALIZED( "<S>" );
            break;
        default:
            break;
    }
    LocStringStream ordinalStr;
    ordinalStr << std::setw( 3 ) << std::setfill( LOCALIZED( '0' ) )
        << ( dataTagDef_ ? dataTagDef_->GetOrdinal() : -1 );
    return containerType + channel_.name + LOCALIZED( "." ) + ordinalStr.str()
        + LOCALIZED( " (" ) + ( dataTagDef_ ? dataTagDef_->GetDescription() : LOCALIZED( "" ) ) + LOCALIZED( ")" );
} // ToString

} // namespace meters

} // namespace logika

/// @file Реализация типа тэга
/// @copyright HypeRRu 2024

#include <logika/meters/data_tag_def.h>

#include <logika/common/misc.h>

namespace logika
{

namespace meters
{

/// Описание тэга с данными

DataTagDef::DataTagDef( const ChannelDef& cdef, const DataTagDefSettings& settings )
    : TagDef( cdef , settings )
    , kind_{ settings.kind }
    , descriptionEx_{ settings.descriptionEx }
    , range_{ settings.range }
    , isBasicParam_{ settings.isBasicParam }
    , updateRate_{ settings.updateRate }
    , address_{ name_ }
{} // DataTagDef


TagKind::Type DataTagDef::GetKind() const
{
    return kind_;
} // GetKind


LocString DataTagDef::GetDescriptionEx() const
{
    return descriptionEx_;
} // GetDescriptionEx


LocString DataTagDef::GetRange() const
{
    return range_;
} // GetRange


bool DataTagDef::IsBasicParam() const
{
    return isBasicParam_;
} // IsBasicParam


uint32_t DataTagDef::GetUpdateRate() const
{
    return updateRate_;
} // GetUpdateRate


int32_t DataTagDef::GetIndex() const
{
    return index_;
} // GetIndex


LocString DataTagDef::GetAddress() const
{
    return address_;
} // GetAddress


LocString DataTagDef::ToString() const
{
    return key_ + LOCALIZED( " " ) + ToLocString( std::to_string( ordinal_ ) )
        + LOCALIZED( " " ) + name_;
} // ToString

/// Хранилище описаний тэгов с данными

DataTagDefVault::DataTagDefVault( const std::vector< std::shared_ptr< DataTagDef > >& tags )
    : dataTagsMap_{}
    , dataTags_{ tags }
{
    for ( auto tag: dataTags_ )
    {
        if ( !tag )
        {
            continue;
        }
        dataTagsMap_.insert( {
              CreateKey( tag->GetChannelDef().prefix, tag->GetName() )
            , tag
        } );
    }
} // DataTagDefVault


const std::shared_ptr< DataTagDef > DataTagDefVault::Find( const LocString& channelPrefix, const LocString& name ) const
{
    auto iter = dataTagsMap_.find( CreateKey( channelPrefix, name ) );
    return dataTagsMap_.cend() != iter ? iter->second : nullptr;
} // Find( const LocString&, const LocString& ) const


std::shared_ptr< DataTagDef > DataTagDefVault::Find( const LocString& channelPrefix, const LocString& name )
{
    auto iter = dataTagsMap_.find( CreateKey( channelPrefix, name ) );
    return dataTagsMap_.end() != iter ? iter->second : nullptr;
} // Find( const LocString&, const LocString& )


const std::vector< std::shared_ptr< DataTagDef > >& DataTagDefVault::All() const
{
    return dataTags_;
} // All() const


LocString DataTagDefVault::CreateKey( const LocString& channelPrefix, const LocString& name )
{
    return channelPrefix + LOCALIZED( "_" ) + name;
} // CreateKey

} // namespace meters

} // namespace logika

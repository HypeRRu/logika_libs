/// @file Реализация базового класса прибора
/// @copyright HypeRRu 2024

#include <logika/meters/meter.h>

#include <logika/meters/data_tag_def.h>
#include <logika/meters/archive_def.h>
#include <logika/meters/archive_field_def.h>
#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <cwctype>
#include <algorithm>
/// @endcond

namespace logika
{

namespace meters
{

Meter::Meter(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : measureKind_{ measureKind }
    , caption_{ caption }
    , description_{ description }
    , maxChannels_{ maxChannels }
    , maxGroups_{ maxGroups }
    , supportedByProlog4_{ false }
    , outdated_{ false }
    , busType_{ busType }
    , tagsVault_{}
    , archiveFields_{}
    , archives_{}
    , channels_{}
    , commonTagDefs_{}
    , ident_{ 0 }
{} // Meter


bool Meter::operator ==( const Meter& other ) const
{
    return      measureKind_    == other.measureKind_
            &&  caption_        == other.caption_
            &&  busType_        == other.busType_;
} // operator ==


bool Meter::operator !=( const Meter& other ) const
{
    return !(*this == other);
} // operator !=


void Meter::Init( const storage::StorageKeeper& sKeeper )
{
    auto channelsStorage = sKeeper.GetStorage< LocString, ChannelDef >();

    if ( channelsStorage && !tagsVault_ )
    {
        const auto keys = channelsStorage->GetKeys();
        std::vector< std::shared_ptr< ChannelDef > > tags;
        for ( const LocString& key: keys )
        {
            std::shared_ptr< ChannelDef > item;
            channelsStorage->GetItem( key, item );
            if ( item && item->meter->GetCaption() == GetCaption() )
            {
                channels_.push_back( item );
            }
        }
    }
} // Init


MeasureKind Meter::GetMeasureKind() const
{
    return measureKind_;
} // GetMeasureKind


LocString Meter::GetCaption() const
{
    return caption_;
} // GetCaption


LocString Meter::GetDescription() const
{
    return description_;
} // GetDescription


uint32_t Meter::GetMaxChannels() const
{
    return maxChannels_;
} // GetMaxChannels


uint32_t Meter::GetMaxGroups() const
{
    return maxGroups_;
} // GetMaxGroups


ChannelKind Meter::GetChannelKind( int32_t cStart, uint32_t cCount, const LocString& cName ) const
{
    return ChannelKind::Undefined;
} // GetChannelKind


LocString Meter::GetVendorId() const
{
    return LOCALIZED( "ЛОГИКА" );
} // GetVendorId


LocString Meter::GetVendor() const
{
    return LOCALIZED( "ЗАО НПФ ЛОГИКА" );
} // GetVendor


BusProtocolType Meter::GetBusType() const
{
    return busType_;
} // GetBusType


LocString Meter::GetEventPrefix( uint32_t tv ) const
{
    (void) tv;
    return LOCALIZED( "" );
} // GetEventPrefix


const std::shared_ptr< DataTagDefVault > Meter::GetTagsVault() const
{
    return tagsVault_;
} // GetTagsVault


const std::vector< std::shared_ptr< ArchiveFieldDef > >& Meter::GetArchiveFields() const
{
    return archiveFields_;
} // GetArchiveFields


const std::vector< std::shared_ptr< ArchiveDef > >& Meter::GetArchives() const
{
    return archives_;
} // GetArchives


const std::vector< std::shared_ptr< ChannelDef > >& Meter::GetChannels() const
{
    return channels_;
} // GetChannels


LocString Meter::GetDisplayFormat( std::shared_ptr< TagDef > def ) const
{
    (void) def;
    return LOCALIZED( "" );
} // GetDisplayFormat


uint16_t Meter::GetIdent() const
{
    return ident_;
} // GetIdent


bool Meter::IdentMatch( ByteType id0, ByteType id1, ByteType version ) const
{
    (void) id0;
    (void) id1;
    (void) version;
    return false;
} // IdentMatch


bool Meter::GetNtFromTag( const LocString& value, ByteType& out ) const
{
    (void) value;
    (void) out;
    return false;
} // GetNtFromTag


const std::unordered_map< ImportantTag, std::vector< LocString > >& Meter::GetCommonTagDefs() const
{
    return commonTagDefs_;
} // GetCommonTagDefs


std::vector< std::shared_ptr< DataTag > > Meter::LookupCommonTags( const std::vector< LocString >& tagDefList ) const
{
    constexpr LocChar separator = LOCALIZED( '.' );

    std::vector< std::shared_ptr< DataTag > > tags;
    tags.reserve( tagDefList.size() );

    for ( const LocString& tagDef: tagDefList )
    {
        int32_t chNo        = -1;
        LocString chType    = LOCALIZED( "" );
        LocString tagName   = LOCALIZED( "" );

        const LocString::size_type sepPos = tagDef.find_first_of( separator );
        if ( LocString::npos == sepPos )
        {
            /// Тэги Logika6
            tagName = tagDef;
            chNo = 0;
            const auto chIter = std::find_if( channels_.cbegin(), channels_.cend(), [](
                const std::shared_ptr< ChannelDef >& channel ) {
                return channel && channel->start == 0 && channel->count == 1;
            } );
            if ( channels_.cend() != chIter )
            {
                chType = ( *chIter )->prefix;
            }
        }
        else if ( LocString::npos == tagDef.find_first_of( separator, sepPos + 1 ) )
        {
            /// Тэги Logika4
            const LocString typeStr = tagDef.substr( 0, sepPos );
            const auto typeIdIter = std::find_if( typeStr.cbegin(), typeStr.cend(), []( const LocChar c ){
                return !std::iswalpha( c );
            } );
            chType = LocString( typeStr.cbegin(), typeIdIter );
            tagName = tagDef.substr( sepPos + 1 );
            chNo = 0;
            if ( typeStr.cend() != typeIdIter )
            {
                uint64_t chan;
                try
                {
                    chan = std::stoull( LocString( typeIdIter + 1, typeStr.cend() ) );
                    chNo = static_cast< int32_t >( chan );
                }
                catch ( const std::exception& )
                {}
            }
        }
        else
        {
            throw std::runtime_error{ "Incorrect common tag address" };
        }
        std::shared_ptr< DataTagDef > def = tagsVault_ ? tagsVault_->Find( chType, tagName ) : nullptr;
        if ( !def )
        {
            throw std::runtime_error{ "Common tag not found" };
        }
        tags.emplace_back(
            std::make_shared< DataTag >( def, chNo )
        );
    }

    return tags;
} // LookupCommonTags


std::unordered_map<
    ImportantTag,
    std::vector< std::shared_ptr< DataTag > >
> Meter::GetWellKnownTags() const
{
    const std::unordered_map< ImportantTag, std::vector< LocString > >& tagDefs = GetCommonTagDefs();
    std::unordered_map< ImportantTag, std::vector< std::shared_ptr< DataTag > > > tags;
    
    for ( const auto& def: tagDefs )
    {
        tags[ def.first ] = LookupCommonTags( def.second );
    }

    return tags;
} // GetWellKnownTags


LocString Meter::ToString() const
{
    return caption_;
} // ToString

} // namespace meters

} // namespace logika

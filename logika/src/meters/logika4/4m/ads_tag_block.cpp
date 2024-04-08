/// @file Реализация типа тэга AdsTagBlock
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/ads_tag_block.h>

namespace logika
{

namespace meters
{

AdsTagBlock::AdsTagBlock( uint32_t id, int32_t channel, uint32_t start, uint32_t count )
    : id_{ id }
    , channels_{}
    , ordinals_{}
{
    channels_.resize( count, channel );
    ordinals_.reserve( count );
    for ( size_t i = 0; i < count; ++i )
    {
        ordinals_.push_back( start + i );
    }
} // AdsTagBlock( uint32_t, int32_t, uint32_t, uint32_t )
    

AdsTagBlock::AdsTagBlock( uint32_t id, const std::vector< LocString >& tags )
    : id_{ id }
    , channels_{}
    , ordinals_{}
{
    constexpr LocChar separator = LOCALIZED( '.' );

    channels_.reserve( tags.size() );
    channels_.reserve( tags.size() );
    for ( const LocString& tag: tags )
    {
        uint64_t chNum  = 0;
        uint64_t ordNum = 0;
        if ( tag.length() > 3 && separator == tag.at( 1 ) )
        {
            try
            {
                chNum = std::stoull( tag.substr( 0, 1 ) );
            }
            catch( const std::exception& e )
            {}
            try
            {
                ordNum = std::stoull( tag.substr( 2 ) );
            }
            catch( const std::exception& e )
            {}
        }
        else
        {
            try
            {
                ordNum = std::stoull( tag );
            }
            catch( const std::exception& e )
            {}
        }
        channels_.push_back( chNum );
        ordinals_.push_back( ordNum );
    }
} // AdsTagBlock( uint32_t, const std::vector< LocString >& )


uint32_t AdsTagBlock::GetId() const
{
    return id_;
} // GetId


const std::vector< int32_t >& AdsTagBlock::GetChannels() const
{
    return channels_;
} // GetChannels


const std::vector< int32_t >& AdsTagBlock::GetOrdinals() const
{
    return ordinals_;
} // GetOrdinals

} // namespace meters

} // namespace logika

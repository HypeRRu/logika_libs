/// @file Реализация класса прибора Logika4
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/logika4.h>

#include <logika/meters/utils/types_converter.h>
#include <logika/common/misc.h>

#include <logika/meters/tag_def.h>

/// @cond
#include <limits>
/// @endcond

namespace logika
{

namespace meters
{

Logika4::Logika4(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : supportBaudRateChange_{ false }
    , maxBaudRate_{ connections::BaudRate::NotSupported }
    , sessionTimeout_{ 0 }
    , supportFastSessionInit_{ false }
    , euMap_{}
    , nsDescriptions_{}
{
    busType_ = BusProtocolType::RSBus;
} // Logika4


LocString Logika4::GetNsDescription( uint32_t index ) const
{
    if ( index >= nsDescriptions_.size() )
    {
        return LOCALIZED( "" );
    }
    return nsDescriptions_.at( index );
} // GetNsDescription( uint32_t index )


LocString Logika4::GetNsDescription( const LocString& ns ) const
{
    /// НСхх+ / ДСхх-
    /// @bug Точно не поддерживается - на конце?
    if ( ns.empty() || ns.at( ns.length() - 1 ) == LOCALIZED( '+' ) )
    {
        return LOCALIZED( "" );
    }
    const LocString::size_type sPos = ns.find( LOCALIZED( "С" ) );
    if ( LocString::npos == sPos || ns.length() - 2 <= sPos )
    {
        return LOCALIZED( "" );
    }
    const LocString nsStr = ns.substr( sPos + 1, ns.length() - 2 - sPos );
    uint64_t index = 0;
    try
    {
        size_t readed = 0;
        index = std::stoull( nsStr, &readed );
        if ( index > std::numeric_limits< uint32_t >::max() || readed != nsStr.length() )
        {
            return LOCALIZED( "" );
        }
    }
    catch( const std::exception& )
    {
        return LOCALIZED( "" );
    }
    return GetNsDescription( static_cast< uint32_t >( index ) );
} // GetNsDescription( const LocString& ns )


LocString Logika4::GetDisplayFormat( std::shared_ptr< TagDef > def ) const
{
    if ( !def )
    {
        return LOCALIZED( "" );
    }
    if ( !def->GetDisplayFormat().empty() )
    {
        return def->GetDisplayFormat();
    }
    switch( def->GetStdVar() )
    {
        case StdVar::G:
        case StdVar::M:
        case StdVar::P:
        case StdVar::V:
        case StdVar::W:
            return LOCALIZED( "0.000" );
        case StdVar::T:
        case StdVar::ti:
            return LOCALIZED( "0.00" );
    }
    return LOCALIZED( "" );
} // GetDisplayFormat


LocString Logika4::GetEventPrefix( uint32_t tv ) const
{
    if ( maxGroups_ == 1 )
    {
        return LOCALIZED( "" );
    }
    return LOCALIZED( "ТВ" ) + ToLocString( std::to_string( tv ) );
} // GetEventPrefix


ChannelKind Logika4::GetChannelKind( int32_t cStart, uint32_t cCount, const LocString& cName ) const
{
    (void) cCount;
    (void) cName;
    return 0 == cStart ? ChannelKind::Common : ChannelKind::TV;
} // GetChannelKind


bool Logika4::GetNtFromTag( const LocString& value, ByteType& out ) const
{
    uint64_t converted;
    try
    {
        size_t readed = 0;
        converted = std::stoull( value, &readed );
        if ( converted > std::numeric_limits< uint8_t >::max() || readed != value.length() )
        {
            return false;
        }
    }
    catch( const std::exception& )
    {
        return false;
    }
    out = static_cast< ByteType >( converted );
    return true;
} // GetNtFromTag


LocString Logika4::GetEu( const LocString& euDef ) const
{
    auto iter = euMap_.find( euDef );
    if ( euMap_.cend() == iter )
    {
        return euDef;
    }
    return iter->second;
} // GetEu


LocString Logika4::DisplayNs( const ByteVector& values )
{
    static const LocString separator = LOCALIZED( "," );
    
    const auto bitNums = BitNumbers( values, values.size() * 8, 0 );
    if ( bitNums.empty() )
    {
        return LOCALIZED( "-" );
    }
    LocString nsString = ToLocString( std::to_string( bitNums.at( 0 ) ) );
    for ( size_t i = 1; i < bitNums.size(); ++i )
    {
        nsString += separator + ToLocString( std::to_string( bitNums.at( i ) ) );
    }
    return nsString;
} // DisplayNs


LocString Logika4::GetGasPressureUnits( uint8_t euValue )
{
    switch ( euValue )
    {
        case 1:
            return LOCALIZED( "МПа" );
        case 2:
            return LOCALIZED( "кгс/см²" );
        case 3:
            return LOCALIZED( "кгс/м²" );
    }
    return LOCALIZED( "кПа" );
} // GetGasPressureUnits


std::shared_ptr< Meter > Logika4::DetermineMeter(
    ByteType id0, ByteType id1, ByteType version,
    std::shared_ptr< storage::Storage< LocString, Meter > > meterStorage
)
{
    if ( !meterStorage )
    {
        return nullptr;
    }
    const auto meterKeys = meterStorage->GetKeys();
    for ( const LocString& key: meterKeys )
    {
        std::shared_ptr< Meter > meter;
        meterStorage->GetItem( key, meter );
        if ( meter && meter->IdentMatch( id0, id1, version ) )
        {
            return meter;
        }
    }
    return nullptr;
} // DetermineMeter


ByteType Logika4::CheckSum8( const ByteVector& buffer, uint32_t start, uint32_t length )
{
    uint8_t cs = 0xFF;
    if ( length > buffer.size() || buffer.size() - length < start )
    {
        return cs;
    }
    for ( size_t i = 0; i < length; ++i )
    {
        cs = static_cast< uint8_t >( cs - buffer.at( start + i ) );
    }
    return static_cast< ByteType >( cs );
} // CheckSum8


bool Logika4::GetSupportBaudRateChange() const
{
    return supportBaudRateChange_;
} // GetSupportBaudRateChange


connections::BaudRate::Type Logika4::GetMaxBaudRate() const
{
    return maxBaudRate_;
} // GetMaxBaudRate


TimeType Logika4::GetSessionTimeout() const
{
    return sessionTimeout_;
} // GetSessionTimeout


bool Logika4::GetSupportFastSessionInit() const
{
    return supportFastSessionInit_;
} // GetSupportFastSessionInit

} // namespace meters

} // namespace logika

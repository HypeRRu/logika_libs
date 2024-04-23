/// @file Реализация класса прибора Logika4M
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/logika4m.h>

#include <logika/meters/utils/types_converter.h>
#include <logika/common/misc.h>

#include <logika/meters/logika4/4m/tag_def4m.h>
#include <logika/meters/logika4/4m/archive_def4m.h>
#include <logika/meters/logika4/4m/archive_field_def4m.h>

/// @cond
#include <algorithm>
#include <iomanip>
/// @endcond

namespace
{

/// @brief Проверка границ данных
void CheckBounds( const logika::ByteVector& buffer, logika::MeterAddressType offset, logika::MeterAddressType length )
{
    if ( offset >= buffer.size() || buffer.size() - offset < length )
    {
        throw std::out_of_range{ "Data out of range" };
    }
} // CheckBounds

} // anonymous namespace


namespace logika
{

namespace meters
{

const LocString Logika4M::ND_STR = LOCALIZED( "#н/д" );

Logika4M::Logika4M(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4( measureKind, caption, description, maxChannels, maxGroups, busType )
    , supportFlz_{ false }
    , supportArchivePartitions_{ false }
{
    supportedByProlog4_ = true;
    supportFastSessionInit_ = true;
} // Logika4M


void Logika4M::Init( const storage::StorageKeeper& sKeeper )
{
    auto tagsStorage            = sKeeper.GetStorage< LocString, TagDef4M >();
    auto archiveStorage         = sKeeper.GetStorage< LocString, ArchiveDef4M >();
    auto archiveFieldStorage    = sKeeper.GetStorage< LocString, ArchiveFieldDef4M >();

    if ( tagsStorage && !tagsVault_ )
    {
        const auto keys = tagsStorage->GetKeys();
        std::vector< std::shared_ptr< DataTagDef > > tags;
        for ( const LocString& key: keys )
        {
            std::shared_ptr< TagDef4M > item;
            tagsStorage->GetItem( key, item );
            if ( item && item->GetMeter()->GetCaption() == GetCaption() )
            {
                tags.push_back( item );
            }
        }
        tagsVault_ = std::make_shared< DataTagDefVault >( tags );
    }

    if ( archiveStorage && archives_.empty() )
    {
        const auto keys = archiveStorage->GetKeys();
        for ( const LocString& key: keys )
        {
            std::shared_ptr< ArchiveDef4M > item;
            archiveStorage->GetItem( key, item );
            if ( item && item->GetMeter()->GetCaption() == GetCaption() )
            {
                archives_.push_back( item );
            }
        }
    }

    if ( archiveFieldStorage && archiveFields_.empty() )
    {
        const auto keys = archiveStorage->GetKeys();
        for ( const LocString& key: keys )
        {
            std::shared_ptr< ArchiveFieldDef4M > item;
            archiveFieldStorage->GetItem( key, item );
            if ( item && item->GetMeter()->GetCaption() == GetCaption() )
            {
                archiveFields_.push_back( item );
            }
        }
    }
} // Init


bool Logika4M::GetSupportFlz() const
{
    return supportFlz_;
} // GetSupportFlz


bool Logika4M::GetSupportArchivePartitions() const
{
    return supportArchivePartitions_;
} // GetSupportArchivePartitions


std::vector< AdsTagBlock > Logika4M::GetAdsTagBlocks() const
{
    return {};
} // GetAdsTagBlocks


MeterAddressType Logika4M::GetTagLength( const ByteVector& buffer
    , MeterAddressType idx, MeterAddressType& length )
{
    CheckBounds( buffer, idx, 1 );
    MeterAddressType tagLen = buffer.at( idx );

    if ( tagLen & 0x80 )
    {
        tagLen &= 0x7F;
        if ( tagLen == 1 )
        {
            CheckBounds( buffer, idx, 2 );
            length = buffer.at( idx + 1 );
        }
        else if ( tagLen == 2 )
        {
            /// @todo Проверить границы
            CheckBounds( buffer, idx, 3 );
            length = ( buffer.at( idx + 1 ) << 8 ) | ( buffer.at( idx + 2 ) );
        }
        else
        {
            throw std::runtime_error{ "length of field >2 bytes" };
        }
        return tagLen + 1;
    }
    else
    {
        length = tagLen;
        return 1;
    }
} // GetTagLength


Logika4M::Tag4MRecordType Logika4M::ParseTag(
    const ByteVector& buffer, MeterAddressType idx, MeterAddressType& readLen )
{
    CheckBounds( buffer, idx, 1 );

    TagId4M::Type tagId = buffer.at( idx );
    MeterAddressType payloadLen;
    MeterAddressType offset = GetTagLength( buffer, idx + 1, payloadLen );
    MeterAddressType payloadOffset = idx + 1 + offset;
    readLen = 1 + offset + payloadLen;
    switch ( tagId )
    {
        case TagId4M::Empty:
            ParseEmptyTag( buffer, idx + 1 + offset, 0 );
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >()
            );
        case TagId4M::Float:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseFloatTag( buffer, payloadOffset, 0 ) )
            );
        case TagId4M::I32LE:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseI32LeTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::ByteArray:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseByteArrayTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Ia5String:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseIa5StringTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Total:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseTotalTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Oper:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseOperTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Ack:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseAckTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Time:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseTimeTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Date:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseDateTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Timestamp:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseTimestampTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::ParamNum:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseParamNumTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::NsRecord:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseNsRecordTag( buffer, payloadOffset, payloadLen ) )
            );
        case TagId4M::Error:
            return std::make_pair(
                tagId,
                std::make_shared< logika::Any >( ParseErrorTag( buffer, payloadOffset, payloadLen ) )
            );
    }
    throw std::runtime_error{ "Unknown tag type " + tagId };
} // ParseTag


void Logika4M::ParseEmptyTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, payloadLen );
} // ParseEmptyTag


float Logika4M::ParseFloatTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::max( payloadLen,
        static_cast< MeterAddressType >( sizeof( float ) ) ) );
    return *( reinterpret_cast< const float* >( buffer.data() + payloadStart ) );
} // ParseFloatTag


uint32_t Logika4M::ParseI32LeTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::min( payloadLen,
        static_cast< MeterAddressType >( sizeof( uint32_t ) ) ) );
    uint32_t data;
    if ( payloadLen == 1 )
    {
        data = *( reinterpret_cast< const uint8_t* >( buffer.data() + payloadStart ) );
    }
    else if ( payloadLen == 2 )
    {
        data = *( reinterpret_cast< const uint16_t* >( buffer.data() + payloadStart ) );
    }
    else if ( payloadLen == 4 )
    {
        data = *( reinterpret_cast< const uint32_t* >( buffer.data() + payloadStart ) );
    }
    else
    {
        throw std::runtime_error{ "Unsupported tag data length" };
    }
    return data;
} // ParseI32LeTag


ByteVector Logika4M::ParseByteArrayTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, payloadLen );
    ByteVector data;
    data.reserve( payloadLen );
    for ( size_t i = 0; i < payloadLen; ++i )
    {
        data.push_back( buffer.at( payloadStart + i ) );
    }
    return data;
} // ParseByteArrayTag


LocString Logika4M::ParseIa5StringTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, payloadLen );
    std::string value( buffer.data(), std::min( static_cast< size_t >( payloadLen ), buffer.size() ) );
    return ToLocString( value );
} // ParseIa5StringTag


double Logika4M::ParseTotalTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::max( payloadLen,
        static_cast< MeterAddressType >( sizeof( uint32_t ) + sizeof( float ) ) ) );

    const uint32_t intPart = *( reinterpret_cast< const uint32_t* >(
        buffer.data() + payloadStart ) );
    const float floatPart  = *( reinterpret_cast< const float* >(
            buffer.data() + payloadStart + sizeof( uint32_t ) ) );
    return static_cast< double >( intPart ) + static_cast< double >( floatPart );
} // ParseTotalTag


OperParamFlag::Type Logika4M::ParseOperTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::min( payloadLen,
        static_cast< MeterAddressType >( sizeof( OperParamFlag::Type ) ) ) );

    if ( payloadLen > 0 )
    {
        return buffer.at( payloadStart ) != 0 ? OperParamFlag::Yes : OperParamFlag::No;
    }
    return OperParamFlag::No;
} // ParseOperTag


ByteType Logika4M::ParseAckTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::min( payloadLen, static_cast< MeterAddressType >( sizeof( ByteType ) ) ) );
    
    if ( payloadLen == 1 )
    {
        return buffer.at( payloadStart );
    }
    return 0;
} // ParseAckTag


LocString Logika4M::ParseTimeTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::max( payloadLen, static_cast< MeterAddressType >( 4 ) ) );

    LocStringStream format;
    format << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) )
        << static_cast< uint32_t >( buffer.at( payloadStart + 3 ) ) << LOCALIZED( "-" );
    format << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) )
        << static_cast< uint32_t >( buffer.at( payloadStart + 2 ) ) << LOCALIZED( "-" );
    format << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) )
        << static_cast< uint32_t >( buffer.at( payloadStart + 1 ) );
    return format.str();
} // ParseTimeTag


LocString Logika4M::ParseDateTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::max( payloadLen, static_cast< MeterAddressType >( 4 ) ) );

    LocStringStream format;
    format << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) )
        << static_cast< uint32_t >( buffer.at( payloadStart + 0 ) ) << LOCALIZED( "-" );
    format << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) )
        << static_cast< uint32_t >( buffer.at( payloadStart + 1 ) ) << LOCALIZED( "-" );
    format << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) )
        << static_cast< uint32_t >( buffer.at( payloadStart + 2 ) );
    return format.str();
} // ParseDateTag


TimeType Logika4M::ParseTimestampTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::min( payloadLen, static_cast< MeterAddressType >( 8 ) ) );
    /// Милисекунды не учитываются
    if ( payloadLen == 0 )
    {
        return 0;
    }
    /// y m d h M s ms_l ms_h
    /// Дата должна быть валидной, остальные поля могут быть пустыми
    ByteVector timeVal{ 0, 1, 1, 0, 0, 0, 0, 0 };
    for ( size_t i = 0; i < payloadLen; ++i )
    {
        timeVal[ i ] = buffer.at( payloadStart + i );
    }
    uint16_t millis = ( timeVal[ 7 ] << 8 ) | timeVal[ 6 ];
    if ( millis > 999 )
    {
        throw std::runtime_error{ "Incorrect millis count: " + std::to_string( millis ) };
    }
    struct tm timeStruct = GetTimeStruct();
    timeStruct.tm_year = timeVal[ 0 ];
    timeStruct.tm_mon  = timeVal[ 1 ] - 1;
    timeStruct.tm_mday = timeVal[ 2 ];
    timeStruct.tm_hour = timeVal[ 3 ];
    timeStruct.tm_min  = timeVal[ 4 ];
    timeStruct.tm_sec  = timeVal[ 5 ];

    return std::mktime( &timeStruct ) * 1000 + millis;
} // ParseTimestampTag


std::pair< ByteType, uint16_t > Logika4M::ParseParamNumTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::max( payloadLen,
        static_cast< MeterAddressType >( sizeof( ByteType ) + sizeof( uint16_t ) ) ) );
    const ByteType ch  = buffer.at( payloadStart );
    const uint16_t ord = *( reinterpret_cast< const uint16_t* >( buffer.data() + payloadStart + 1 ) );
    return std::make_pair( ch, ord );
} // ParseParamNumTag


std::vector< size_t > Logika4M::ParseNsRecordTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, static_cast< MeterAddressType >( payloadLen ) );
    if ( payloadLen <= 16 )
    {
        /// @todo Check correctness
        return BitNumbers( buffer, payloadStart * 8, payloadLen * 8 );
    }
    throw std::runtime_error{ "FLAGS tag length unsupported" };
} // ParseNsRecordTag


ByteType Logika4M::ParseErrorTag(
    const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen )
{
    CheckBounds( buffer, payloadStart, std::max( payloadLen, static_cast< MeterAddressType >( 1 ) ) );
    return buffer.at( payloadStart );
} // ParseErrorTag

} // namespace meters

} // namespace logika

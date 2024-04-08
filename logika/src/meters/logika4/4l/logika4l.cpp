/// @file Реализация класса прибора Logika4L
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/logika4l.h>

#include <logika/meters/utils/types_converter.h>
#include <logika/common/misc.h>

#include <logika/meters/logika4/4l/tag_def4l.h>
#include <logika/meters/logika4/4l/archive_def4l.h>
#include <logika/meters/logika4/4l/archive_field_def4l.h>

/// @cond
#include <limits>
#include <stdexcept>
#include <array>
#include <cwctype>
#include <iomanip>
/// @endcond

namespace logika
{

namespace meters
{

Logika4L::Logika4L(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    supportedByProlog4_ = true;
    outdated_ = true;
} // Logika4L


void Logika4L::Init( const storage::StorageKeeper& sKeeper )
{
    Logika4::Init( sKeeper );

    auto tagsStorage            = sKeeper.GetStorage< LocString, TagDef4L >();
    auto archiveStorage         = sKeeper.GetStorage< LocString, ArchiveDef4L >();
    auto archiveFieldStorage    = sKeeper.GetStorage< LocString, ArchiveFieldDef4L >();

    if ( tagsStorage && !tagsVault_ )
    {
        const auto keys = tagsStorage->GetKeys();
        std::vector< std::shared_ptr< DataTagDef > > tags;
        for ( const LocString& key: keys )
        {
            std::shared_ptr< TagDef4L > item;
            tagsStorage->GetItem( key, item );
            if ( item )
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
            std::shared_ptr< ArchiveDef4L > item;
            archiveStorage->GetItem( key, item );
            if ( item )
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
            std::shared_ptr< ArchiveFieldDef4L > item;
            archiveFieldStorage->GetItem( key, item );
            if ( item )
            {
                archiveFields_.push_back( item );
            }
        }
    }
} // Init


LocString Logika4L::GetModelFromImage( const ByteVector& flashImage ) const
{
    return LOCALIZED( "" );
} // GetModelFromImage


std::vector< Logika4L::AdsFlashRun > Logika4L::GetAdsFileLayout( bool all, const LocString& model ) const
{
    return {};
} // GetAdsFileLayout


MeterAddressType Logika4L::SizeOfType( BinaryType4L::Type dataType )
{
    switch ( dataType )
    {
        case BinaryType4L::U8:
        case BinaryType4L::BitArray8:
        case BinaryType4L::ModelChar:
            return 1;

        case BinaryType4L::BitArray16:
        case BinaryType4L::MmHhTime:
            return 2;

        case BinaryType4L::BitArray24:
            return 3;

        case BinaryType4L::BitArray32:
        case BinaryType4L::Time:        ///< HH MM SS + 1 байт
        case BinaryType4L::Date:        ///< YY MM DD + 1 байт
        case BinaryType4L::MmDdDate:    ///< MM-DD-xx-xx
        case BinaryType4L::R32:
        case BinaryType4L::U24:         ///< 3 байта значение + 1 байт выравнивание
            return 4;

        case BinaryType4L::ServiceRecordTimestamp:  ///< контрольный байт + yy mm dd hh MM
            return 6;

        case BinaryType4L::I32R32:
        case BinaryType4L::NsRecord:
            return 8;
        
        case BinaryType4L::TripleR32:
            return 12;
        
        case BinaryType4L::DbEntry:
        case BinaryType4L::DbEntryByte:
            return 16;
        
        case BinaryType4L::IzmRecord:
            return 24;

        // case BinaryType4L::ArchiveStruct:
    }
    /// @bug ArchiveStruct отсутствует?
    throw std::runtime_error{ "Unknown data type" };
} // SizeOfType


float Logika4L::ConvertMFloat( const ByteVector& buffer, MeterAddressType offset )
{
    if ( offset >= buffer.size() || buffer.size() - offset < sizeof( float ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }

    uint32_t uintRep = *( reinterpret_cast< const uint32_t* >( buffer.data() + offset ) );
    uint32_t sign = ( uintRep >> 23 ) & 1;
    uint32_t exponent = uintRep >> 24;
    uint32_t mantissa = uintRep & 0x007FFFFF;
    /// Конвертация
    uintRep = ( exponent << 23 ) | ( sign << 31 ) | mantissa;
    /// Проверка на exponent = NaN
    if ( ( uintRep & 0x7F800000 ) == 0x7F800000 )
    {
        uintRep &= 0xFF7FFFFF; /// Очищается LSB экспоненты
    }
    return static_cast< float >( uintRep );
} // ConvertMFloat


TimeType Logika4L::SyncHeaderToDateTime( std::shared_ptr< ArchiveType > arType
    , ByteType rd, ByteType rh, const ByteVector& buffer, MeterAddressType offset )
{
    constexpr size_t headerSize = 4;
    const LocString arTypeName = arType ? arType->GetName() : LOCALIZED( "?" );
    if ( offset >= buffer.size() || buffer.size() - offset < headerSize )
    {
        return 0;
    }

    uint8_t year    = static_cast< uint8_t >( buffer.at( offset + 0 ) );
    uint8_t mon     = static_cast< uint8_t >( buffer.at( offset + 1 ) );
    uint8_t day     = arTypeName == LOCALIZED( "Month" ) ? rd : static_cast< uint8_t >( buffer.at( offset + 2 ) );
    uint8_t hour    = arTypeName == LOCALIZED( "Hour"  ) ? static_cast< uint8_t >( buffer.at( offset + 3 ) ) : rh;

    if ( mon == 0 || mon > 12 || day == 0 || day > 31 || hour > 23 )
    {
        return 0;
    }

    struct tm timeStruct = GetTimeStruct();
    timeStruct.tm_sec   = 0;
    timeStruct.tm_min   = 0;
    timeStruct.tm_hour  = hour;
    timeStruct.tm_mday  = day;
    timeStruct.tm_mon   = mon - 1;
    timeStruct.tm_year  = year;

    return std::mktime( &timeStruct );
} // SyncHeaderToDateTime


LocString Logika4L::LcdCharsToString( const ByteVector& buffer, MeterAddressType offset, MeterAddressType length )
{
    const size_t bufSize = buffer.size() / sizeof( LocChar );
    if ( offset >= bufSize || bufSize - offset < length )
    {
        return LOCALIZED( "" );
    }

    LocString result;
    result.resize( length );
    const LocChar* buf = reinterpret_cast< const LocChar* >( buffer.data() );

    for ( size_t i = 0; i < length; ++i )
    {
        LocChar rCh = buf[ i ];
        result[ i ] = std::iswgraph( rCh ) ? rCh : LOCALIZED( ' ' );
    }
    return result;
} // LcdCharsToString


std::unique_ptr< logika::Any > Logika4L::GetValue( BinaryType4L::Type binType,
    const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    switch ( binType )
    {
        case BinaryType4L::R32:
            return std::make_unique< logika::Any >( GetR32Value( buffer, offset, oper ) );
        case BinaryType4L::TripleR32:
            return std::make_unique< logika::Any >( GetTripleR32Value( buffer, offset, oper ) );
        case BinaryType4L::Time:
            return std::make_unique< logika::Any >( GetTimeValue( buffer, offset, oper ) );
        case BinaryType4L::Date:
            return std::make_unique< logika::Any >( GetDateValue( buffer, offset, oper ) );
        case BinaryType4L::MmDdDate:
            return std::make_unique< logika::Any >( GetMmDdValue( buffer, offset, oper ) );
        case BinaryType4L::BitArray32:
            return std::make_unique< logika::Any >( GetBitArray32Value( buffer, offset, oper ) );
        case BinaryType4L::BitArray24:
            return std::make_unique< logika::Any >( GetBitArray24Value( buffer, offset, oper ) );
        case BinaryType4L::BitArray16:
            return std::make_unique< logika::Any >( GetBitArray16Value( buffer, offset, oper ) );
        case BinaryType4L::BitArray8:
            return std::make_unique< logika::Any >( GetBitArray8Value( buffer, offset, oper ) );
        case BinaryType4L::DbEntry:
            return std::make_unique< logika::Any >( GetDbEntryValue( buffer, offset, oper ) );
        case BinaryType4L::DbEntryByte:
            return std::make_unique< logika::Any >( GetDbEntryByteValue( buffer, offset, oper ) );
        case BinaryType4L::U8:
            return std::make_unique< logika::Any >( GetU8Value( buffer, offset, oper ) );
        case BinaryType4L::I32R32:
            return std::make_unique< logika::Any >( GetI32R32Value( buffer, offset, oper ) );
        case BinaryType4L::MmHhTime:
            return std::make_unique< logika::Any >( GetMmHhValue( buffer, offset, oper ) );
        case BinaryType4L::NsRecord:
            return std::make_unique< logika::Any >( GetNsRecordValue( buffer, offset, oper ) );
        case BinaryType4L::IzmRecord:
            return std::make_unique< logika::Any >( GetIzmRecordValue( buffer, offset, oper ) );
        // case BinaryType4L::ArchiveStruct:
            // return std::make_unique< logika::Any >(  );
        case BinaryType4L::ModelChar:
            return std::make_unique< logika::Any >( GetModelCharValue( buffer, offset, oper ) );
        case BinaryType4L::U24:
            return std::make_unique< logika::Any >( GetU24Value( buffer, offset, oper ) );
        case BinaryType4L::ServiceRecordTimestamp:
            return std::make_unique< logika::Any >( GetSRTimestampValue( buffer, offset, oper ) );
    }
    return nullptr;
} // GetValue


float Logika4L::GetR32Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    oper = false;
    return ConvertMFloat( buffer, offset );
} // GetR32Value


LocString Logika4L::GetTimeValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::Time ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;

    struct tm timeStruct = GetTimeStruct();
    timeStruct.tm_hour  = buffer.at( offset + 0 );
    timeStruct.tm_min   = buffer.at( offset + 1 );
    timeStruct.tm_sec   = buffer.at( offset + 2 );
    return GetFormatTime( timeStruct, "%H:%M:%S", sizeof( "HH:MM:SS" ) );
} // GetTimeValue


LocString Logika4L::GetDateValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::Date ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;

    struct tm timeStruct = GetTimeStruct();
    /// Год считается от 2000, не от 1900
    timeStruct.tm_year  = static_cast< int >( buffer.at( offset + 0 ) ) + 100;
    timeStruct.tm_mon   = buffer.at( offset + 1 ) - 1;
    timeStruct.tm_mday  = buffer.at( offset + 2 );
    return GetFormatTime( timeStruct, "%d/%m/%y", sizeof( "dd/mm/yy" ) );
} // GetDateValue


LocString Logika4L::GetMmDdValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::MmDdDate ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;

    struct tm timeStruct = GetTimeStruct();
    /// Год считается от 2000, не от 1900
    timeStruct.tm_mon   = buffer.at( offset + 0 ) - 1;
    timeStruct.tm_mday  = buffer.at( offset + 1 );
    return GetFormatTime( timeStruct, "%d/%m", sizeof( "dd/mm" ) );
} // GetMmDdValue


std::vector< size_t > Logika4L::GetBitArray8Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::BitArray8 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    return BitNumbers( buffer, 8, offset * 8 );
} // GetBitArray8Value


std::vector< size_t > Logika4L::GetBitArray16Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::BitArray16 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    return BitNumbers( buffer, 16, offset * 8 );
} // GetBitArray16Value


std::vector< size_t > Logika4L::GetBitArray24Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::BitArray24 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    return BitNumbers( buffer, 24, offset * 8 );
} // GetBitArray24Value


std::vector< size_t > Logika4L::GetBitArray32Value(  const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::BitArray32 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    return BitNumbers( buffer, 32, offset * 8 );
} // GetBitArray32Value


LocString Logika4L::GetDbEntryValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    constexpr MeterAddressType PARAM_BIN_PART_LEN = 4;
    constexpr MeterAddressType PARAM_STR_PART_LEN = 8;
    const MeterAddressType strPartOffset = offset + PARAM_BIN_PART_LEN;
    
    oper = false;

    /// @bug Нужно здесь проверять размер на 12 или 16?
    if ( strPartOffset >= buffer.size() || buffer.size() - strPartOffset < SizeOfType( BinaryType4L::DbEntry ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }

    /// для некоторых приборов в новых версиях прошивок добавлялись настроечные параметры
    /// => на приборах со старыми прошивками эти параметры не инициализированы (строковая часть заполнена 0xFF)
    if ( static_cast< uint8_t >( buffer.at( strPartOffset ) ) == 0xFF )
    {
        return LOCALIZED( "" );
    }
    
    oper = ( buffer.at( offset ) & 0x01 ) != 0;
    const std::string paramStr{ buffer.data() + strPartOffset, PARAM_STR_PART_LEN };

    return Trim( ToLocString( paramStr ) );
} // GetDbEntryValue


ByteType Logika4L::GetDbEntryByteValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::DbEntryByte ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = ( buffer.at( offset ) & 0x01 ) != 0;
    return buffer.at( offset + 12 );
} // GetDbEntryByteValue


ByteType Logika4L::GetU8Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::U8 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    return buffer.at( offset );
} // GetU8Value


double Logika4L::GetTripleR32Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::TripleR32 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    const double v1 = ConvertMFloat( buffer, offset + 0 );
    const double v2 = ConvertMFloat( buffer, offset + 4 );
    const double v3 = ConvertMFloat( buffer, offset + 8 );
    return v1 + v2 + v3;
} // GetTripleR32Value


double Logika4L::GetI32R32Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::I32R32 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    const int32_t intPart  = *( reinterpret_cast< const int32_t* > ( buffer.data() + offset ) );
    const double floatPart = ConvertMFloat( buffer, offset + 4 );
    return static_cast< double >( intPart ) + floatPart;
} // GetI32R32Value


LocString Logika4L::GetMmHhValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::MmHhTime ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;

    struct tm timeStruct = GetTimeStruct();
    timeStruct.tm_hour  = buffer.at( offset + 1 );
    timeStruct.tm_min   = buffer.at( offset + 0 );
    return GetFormatTime( timeStruct, "%H:%M", sizeof( "HH:MM" ) );
} // GetMmHhValue


uint32_t Logika4L::GetU24Value( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::U24 ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    const uint32_t result = *( reinterpret_cast< const uint32_t* > ( buffer.data() + offset ) );
    return result & 0x00FFFFFF;
} // GetU24Value


ByteType Logika4L::GetModelCharValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::ModelChar ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    return buffer.at( offset );
} // GetModelCharValue


TimeType Logika4L::GetSRTimestampValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::ServiceRecordTimestamp ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;

    if ( buffer.at( offset ) != 0x10 )
    {
        return 0;
    }

    struct tm timeStruct = GetTimeStruct();
    /// Год считается от 2000, не от 1900
    timeStruct.tm_year  = static_cast< int >( buffer.at( offset + 1 ) ) + 100;
    timeStruct.tm_mon   = buffer.at( offset + 2 ) - 1;
    timeStruct.tm_mday  = buffer.at( offset + 3 );
    timeStruct.tm_hour  = buffer.at( offset + 4 );
    timeStruct.tm_min   = buffer.at( offset + 5 );
    timeStruct.tm_sec   = 0;

    if (   timeStruct.tm_year - 100 == 0xFF
        || timeStruct.tm_mon  == 0 || timeStruct.tm_mon  > 12
        || timeStruct.tm_mday == 0 || timeStruct.tm_mday > 31
        || timeStruct.tm_hour > 23 || timeStruct.tm_min  > 59 )
    {
        return 0;
    }

    return std::mktime( &timeStruct );
} // GetSRTimestampValue


LocString Logika4L::GetNsRecordValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::NsRecord ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }
    oper = false;
    
    LocStringStream numFormat;
    numFormat << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) ) << buffer.at( offset + 6 );

    /// Номер нештатной ситуации и "+/-"
    return LOCALIZED( "НС" ) + numFormat.str() + 
        ( ( buffer.at( offset + 7 ) & 0x01 > 0 ) ? LOCALIZED( "+" ) : LOCALIZED( "-" ) );
} // GetNsRecordValue


LocString Logika4L::GetIzmRecordValue( const ByteVector& buffer, MeterAddressType offset, bool& oper )
{
    if ( offset >= buffer.size() || buffer.size() - offset < SizeOfType( BinaryType4L::IzmRecord ) )
    {
        throw std::out_of_range{ "Data out of range" };
    }

    return Trim( LcdCharsToString( buffer, offset + 8, SizeOfType( BinaryType4L::IzmRecord ) - 8 ) );
} // GetIzmRecordValue

} // namespace meters

} // namespace logika

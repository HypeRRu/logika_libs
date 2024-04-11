/// @file Реализация класса прибора SPT941
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4l/spt941.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

/// @cond
#include <algorithm>
#include <limits>
/// @endcond

namespace logika
{

namespace meters
{

Spt941::Spt941(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4L( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x5429;
    supportBaudRateChange_ = false;
    supportedByProlog4_ = false;
    maxBaudRate_ = connections::BaudRate::Rate2400;
    sessionTimeout_ = 30 * 60 * 1000; /// 30 минут
    supportFastSessionInit_ = false;

    commonTagDefs_ = {
        { ImportantTag::NetAddr, { LOCALIZED( "ОБЩ.NT" ) } },
        { ImportantTag::Ident,   { LOCALIZED( "ОБЩ.ИД" ) } }
    };

    nsDescriptions_ = {
          LOCALIZED( "Разряд батареи" ) // 00
        , LOCALIZED( "Выход температуры на ТС1 за диапазон" ) // 01
        , LOCALIZED( "Выход температуры на ТС2 за диапазон" ) // 02
        , LOCALIZED( "Перегрузка цепи питания ВС" ) // 03
        , LOCALIZED( "Масса М3 (ГВС) меньше минус 0,04 М1" ) // 04
        , LOCALIZED( "Отрицательная тепловая энергия" ) // 05
    };
} // Spt941


std::vector< float > Spt941::ExpandHourRecord( const ByteVector& hourRec )
{
    /// @note Если значение не задано, то оно задается
    /// как std::numeric_limits< float >::infinity()
    constexpr float  recordNull = std::numeric_limits< float >::infinity();
    constexpr size_t recordSize = 12;

    std::vector< float > normRec;
    normRec.resize( recordSize );

    //SP NS T1 T2 перемещать не нужно
    for ( size_t i = 0; i < 4; ++i )
    {
        normRec[ i ] = static_cast< float >( hourRec.at( i ) );
    }

    ByteType sp = hourRec.at( 0 );
    float v12   = static_cast< float >( hourRec.at( 4 ) );
    float v23   = static_cast< float >( hourRec.at( 5 ) );
    float m12   = static_cast< float >( hourRec.at( 6 ) );
    float m23   = static_cast< float >( hourRec.at( 7 ) );
    float q     = static_cast< float >( hourRec.at( 8 ) );

    switch ( sp )
    {
        case 0:
            normRec[ 4 ] = v12;
            normRec[ 5 ] = v23;
            normRec[ 6 ] = recordNull;
            normRec[ 7 ] = m12;
            normRec[ 8 ] = m23;
            normRec[ 9 ] = m12 - m23;
            break;
        case 1:
            normRec[ 4 ] = v12;
            normRec[ 5 ] = recordNull;
            normRec[ 6 ] = v23;
            normRec[ 7 ] = m12;
            normRec[ 8 ] = m23;
            normRec[ 9 ] = m12 - m23;
            break;
        case 2:
            normRec[ 4 ] = recordNull;
            normRec[ 5 ] = v12;
            normRec[ 6 ] = v23;
            normRec[ 7 ] = m12;
            normRec[ 8 ] = m23;
            normRec[ 9 ] = m12 - m23;
            break;
        case 3:
        case 4:
            normRec[ 4 ] = v12;
            normRec[ 5 ] = v23;
            normRec[ 6 ] = recordNull;
            normRec[ 7 ] = m12;
            normRec[ 8 ] = m23;
            normRec[ 9 ] = recordNull;
            break;
        case 5:
            normRec[ 4 ] = v12;
            normRec[ 5 ] = recordNull;
            normRec[ 6 ] = v23;
            normRec[ 7 ] = m12;
            normRec[ 8 ] = recordNull;
            normRec[ 9 ] = m23;
            break;
        case 6:
            normRec[ 4 ] = recordNull;
            normRec[ 5 ] = v12;
            normRec[ 6 ] = v23;
            normRec[ 7 ] = recordNull;
            normRec[ 8 ] = m12;
            normRec[ 9 ] = m23;
            break;
        case 7:
            normRec[ 3 ] = recordNull;
            normRec[ 4 ] = v12;
            normRec[ 5 ] = recordNull;
            normRec[ 6 ] = recordNull;
            normRec[ 7 ] = m12;
            normRec[ 8 ] = recordNull;
            normRec[ 9 ] = recordNull;
            break;
        case 8:
            normRec[ 4 ] = v12;
            normRec[ 5 ] = v23;
            normRec[ 6 ] = recordNull;
            normRec[ 7 ] = m12;
            normRec[ 8 ] = recordNull;
            normRec[ 9 ] = recordNull;
            break;
        case 9:
            normRec[ 4 ] = v12;
            normRec[ 5 ] = v23;
            normRec[ 6 ] = recordNull;
            normRec[ 7 ] = recordNull;
            normRec[ 8 ] = recordNull;
            normRec[ 9 ] = recordNull;
            break;
        default:
            normRec[ 4 ] = recordNull;
            normRec[ 5 ] = recordNull;
            normRec[ 6 ] = recordNull;
            normRec[ 7 ] = recordNull;
            normRec[ 8 ] = recordNull;
            normRec[ 9 ] = recordNull;
            break;
    }
    normRec[ 10 ] = q;
    normRec[ 11 ] = recordNull;

    return normRec;
} // ExpandHourRecord


std::unordered_map< LocString, LocString > Spt941::BuildEuDict(
    const std::vector< std::shared_ptr< DataTag > >& euTags ) const
{
    /// @bug throw not implemented
    return {};
} // BuildEuDict


LocString Spt941::GetModelFromImage( const ByteVector& flashImage ) const
{
    return LOCALIZED( "" );
} // GetModelFromImage


std::vector< Logika4L::AdsFlashRun > Spt941::GetAdsFileLayout( bool all, const LocString& model ) const
{
    (void) model;
    if ( all )
    {
        return {
            Logika4L::AdsFlashRun{ 0x00000, 0x0D880 }
        };
    }
    else
    {
        return {
              Logika4L::AdsFlashRun{ 0x00000, 0x01700 }
            , Logika4L::AdsFlashRun{ 0x09E00, 0x03A80 }
        };
    }
} // GetAdsFileLayout

} // namespace meters

} // namespace logika

/// @file Реализация тэга с данными
/// @copyright HypeRRu 2024

#include <logika/meters/data_tag.h>

#include <logika/common/misc.h>

/// @cond
#include <sstream>
#include <iomanip>
/// @endcond

namespace logika
{

namespace meters
{

DataTag::DataTag( std::shared_ptr< DataTagDef > def, int32_t channelNo )
    : Tag( def, channelNo )
    , value_{}
    , timestamp_{ 0 }
    , oper_{ false }
    , errDesc_{ LOCALIZED( "" ) }
    , dataTagDef_{ def }
{
    /// @bug Будет ли все правильно работать?
    address_ = LOCALIZED( "" );
    if ( channelNo > 0 )
    {
        address_ = ( dataTagDef_ ? dataTagDef_->GetChannelDef().prefix : LOCALIZED( "" ) )
            + ToLocString( std::to_string( channelNo ) ) + LOCALIZED( "_" );
    }
    address_ += ( dataTagDef_ ? dataTagDef_->GetAddress() : LOCALIZED( "" ) );
} // DataTag


bool DataTag::HasValue() const
{
    return value_ && !value_->Empty();
} // HasValue


void DataTag::ReplaceValue( std::shared_ptr< logika::Any > value )
{
    value_ = value;
} // ReplaceValue


int32_t DataTag::GetIndex() const
{
    return ( dataTagDef_ ? dataTagDef_->GetIndex() : -1 );
} // GetIndex


TimeType DataTag::GetTimestamp() const
{
    return timestamp_;
} // GetTimestamp


void DataTag::SetTimestamp( TimeType stamp )
{
    timestamp_ = stamp;
} // SetTimestamp


bool DataTag::GetOper() const
{
    return oper_;
} // GetOper


void DataTag::SetOper( bool oper )
{
    oper_ = oper;
} // SetOper


LocString DataTag::GetErrorDescription() const
{
    return errDesc_;
} // GetErrorDescription


void DataTag::SetErrorDescription( const LocString& description )
{
    errDesc_ = description;
} // SetErrorDescription


LocString DataTag::GetDisplayFormat() const
{
    return ( dataTagDef_ ? dataTagDef_->GetDisplayFormat() : LOCALIZED( "" ) );
} // GetDisplayFormat


LocString DataTag::ToString() const
{
    int32_t idx = GetIndex();
    LocStringStream idxStr;
    if ( idx != -1 )
    {
        idxStr << '#';
        idxStr << std::setw( 2 ) << std::setfill( LOCALIZED( '0' ) ) << idx;
    }
    LocString euStr = Trim( eu_ );
    if ( !euStr.empty() )
    {
        euStr = LOCALIZED( "[" ) + euStr + LOCALIZED( "]" );
    }
    LocStringStream ordinalStr;
    ordinalStr << std::setw( 3 ) << std::setfill( LOCALIZED( '0' ) )
        << ( dataTagDef_ ? dataTagDef_->GetOrdinal() : -1 );
    return channel_.name + LOCALIZED( "." ) + ordinalStr.str()
        + idxStr.str() + LOCALIZED( "(" )
        + ( dataTagDef_ ? dataTagDef_->GetName() : LOCALIZED( "" ) ) + LOCALIZED( ") " )
        + ( !HasValue() ? LOCALIZED( "without" ) : LOCALIZED( "with" ) )
        + LOCALIZED( " value " ) + euStr;
} // ToString

} // namespace meters

} // namespace logika

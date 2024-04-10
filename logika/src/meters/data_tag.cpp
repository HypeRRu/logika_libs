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
    , errDesc_{ L"" }
    , dataTagDef_{ def }
{
    /// @bug Будет ли все правильно работать?
    address_ = L"";
    if ( channelNo > 0 )
    {
        address_ = ( dataTagDef_ ? dataTagDef_->GetChannelDef().prefix : L"" )
            + ToLocString( std::to_string( channelNo ) ) + L"_";
    }
    address_ += ( dataTagDef_ ? dataTagDef_->GetAddress() : L"" );
} // DataTag


bool DataTag::HasValue() const
{
    return !value_.Empty();
} // HasValue


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
    return ( dataTagDef_ ? dataTagDef_->GetDisplayFormat() : L"" );
} // GetDisplayFormat


LocString DataTag::ToString() const
{
    int32_t idx = GetIndex();
    LocStringStream idxStr;
    if ( idx != -1 )
    {
        idxStr << '#';
        idxStr << std::setw( 2 ) << std::setfill( L'0' ) << idx;
    }
    LocString euStr = Trim( eu_ );
    if ( !euStr.empty() )
    {
        euStr = L"[" + euStr + L"]";
    }
    LocStringStream ordinalStr;
    ordinalStr << std::setw( 3 ) << std::setfill( L'0' ) << ( dataTagDef_ ? dataTagDef_->GetOrdinal() : -1 );
    return channel_.name + L"." + ordinalStr.str()
        + idxStr.str() + L"(" + ( dataTagDef_ ? dataTagDef_->GetName() : L"" ) + L") "
        + ( value_.Empty() ? L"without" : L"with" ) + L" value " + euStr;
} // ToString

} // namespace meters

} // namespace logika

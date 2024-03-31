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

DataTag::DataTag( const DataTagDef& def, int32_t channelNo )
    : Tag( def, channelNo )
    , value_{}
    , timestamp_{ 0 }
    , oper_{ false }
    , errDesc_{ "" }
    , dataTagDef_{ def }
{
    /// @bug Будет ли все правильно работать?
    address_ = "";
    if ( channelNo > 0 )
    {
        address_ = dataTagDef_.GetChannelDef().prefix + std::to_string( channelNo ) + "_";
    }
    address_ += dataTagDef_.GetAddress();
} // DataTag


int32_t DataTag::GetIndex() const
{
    return dataTagDef_.GetIndex();
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


std::string DataTag::GetErrorDescription() const
{
    return errDesc_;
} // GetErrorDescription


void DataTag::SetErrorDescription( const std::string& description )
{
    errDesc_ = description;
} // SetErrorDescription


std::string DataTag::GetDisplayFormat() const
{
    return dataTagDef_.GetDisplayFormat();
} // GetDisplayFormat


std::string DataTag::ToString() const
{
    int32_t idx = GetIndex();
    std::stringstream idxStr;
    if ( idx != -1 )
    {
        idxStr << '#';
        idxStr << std::setw( 2 ) << std::setfill( '0' ) << idx;
    }
    std::string euStr = Trim( eu_ );
    if ( !euStr.empty() )
    {
        euStr = "[" + euStr + "]";
    }
    std::stringstream ordinalStr;
    ordinalStr << std::setw( 3 ) << std::setfill( '0' ) << dataTagDef_.GetOrdinal();
    return channel_.name + "." + ordinalStr.str()
        + idxStr.str() + "(" + dataTagDef_.GetName() + ") "
        + ( value_.Empty() ? "without" : "with" ) + " value " + euStr;
} // ToString

} // namespace meters

} // namespace logika

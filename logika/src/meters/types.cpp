/// @file Реализация типов для работы с приборами
/// @copyright HypeRRu 2024

#include <logika/meters/types.h>

#include <logika/common/misc.h>

namespace logika
{

namespace meters
{

/// Тип архива

ArchiveType::ArchiveType( const LocString& name, const LocString& description
    , ArchiveTimingType timing, const LocString& acronym, TimeType interval, bool variableInterval )
    : timing_{ timing }
    , name_{ name }
    , acronym_{ acronym }
    , description_{ description }
    , interval_{ interval }
    , variableInterval_{ variableInterval }
{} // ArchiveType


ArchiveTimingType ArchiveType::GetTimingType() const
{
    return timing_;
} // GetTimingType


const LocString& ArchiveType::GetName() const
{
    return name_;
} // GetName


const LocString& ArchiveType::GetAcronym() const
{
    return acronym_;
} // GetAcronym


const LocString& ArchiveType::GetDescription() const
{
    return description_;
} // GetDescription


TimeType ArchiveType::GetInterval() const
{
    return interval_;
} // GetInterval


bool ArchiveType::IsIntervalArchive() const
{
    return timing_ == ArchiveTimingType::Synchronous;
} // IsIntervalArchive


bool ArchiveType::IsServiceArchive() const
{
    return timing_ == ArchiveTimingType::Asynchronous;
} // IsServiceArchive


LocString ArchiveType::ToString() const
{
    return name_;
} // ToString


/// Тип, состояший из метрики, ее значения и метки времени

VQT::VQT()
    : VQT( nullptr, 0, 0 )
{} // VQT()


VQT::VQT( ISerializable* v, int32_t q, TimeType t )
    : value{ v }
    , quality{ q }
    , timestamp{ t }
{} // VQT( ... )


LocString VQT::ToString() const
{
    return GetTimeString( timestamp ) + L" - "
        + ( value ? value->ToString() : L"[null]" )
        + L": " + ToLocString( std::to_string( quality ) );
} // ToString


/// Информация о столбце БД

LocString ColumnInfo::ToString() const
{
    return name;
} // ToString


bool ColumnInfo::operator ==( const ColumnInfo& other )
{
    return StrCaseEq( name, other.name )
        && StrCaseEq( dataType, other.dataType )
        && nullable == other.nullable;
} // operator ==

bool ColumnInfo::operator !=( const ColumnInfo& other )
{
    return !( *this == other );
} // operator !=

} // namespace meters

} // namespace logika

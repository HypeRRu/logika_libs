/// @file Реализация типов для работы с приборами
/// @copyright HypeRRu 2024

#include <logika/meters/types.h>

#include <logika/common/misc.h>

namespace logika
{

namespace meters
{

/// Тип архива

std::unordered_map< std::string, ArchiveType* > ArchiveType::types_{};

ArchiveType::ArchiveType( const std::string& name, const std::string& description
    , ArchiveTimingType timing, const std::string& acronym, TimeType interval )
    : timing_{ timing }
    , name_{ name }
    , acronym_{ acronym }
    , description_{ description }
    , interval_{ interval }
{
    types_[ name ] = this;
} // ArchiveType


const std::unordered_map< std::string, ArchiveType* >& ArchiveType::All()
{
    return types_;
} // All


ArchiveTimingType ArchiveType::GetTimingType() const
{
    return timing_;
} // GetTimingType


const std::string& ArchiveType::GetName() const
{
    return name_;
} // GetName


const std::string& ArchiveType::GetAcronym() const
{
    return acronym_;
} // GetAcronym


const std::string& ArchiveType::GetDescription() const
{
    return description_;
} // GetDescription


TimeType ArchiveType::GetInterval() const
{
    return interval_;
} // GetInterval


/// Тип, состояший из метрики, ее значения и метки времени

VQT::VQT()
    : VQT( nullptr, 0, 0 )
{} // VQT()


VQT::VQT( ISerializable* v, int32_t q, TimeType t )
    : value{ v }
    , quality{ q }
    , timestamp{ t }
{} // VQT( ... )


std::string VQT::ToString() const
{
    return GetTimeString( timestamp ) + " - "
        + ( value ? value->ToString() : "[null]" )
        + ": " + std::to_string( quality );
} // ToString


/// Информация о столбце БД

std::string ColumnInfo::ToString() const
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

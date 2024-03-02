/// @file Реализация типов для работы с приборами
/// @copyright HypeRRu 2024

#include <logika/meters/types.h>

#include <logika/common/misc.h>

namespace logika
{

namespace meters
{

/// Тип архива

// std::unordered_map< std::string, const ArchiveType* > ArchiveType::types_{};

ArchiveType::ArchiveType( const std::string& name, const LocString& description
    , ArchiveTimingType timing, const LocString& acronym, TimeType interval, bool variableInterval )
    : timing_{ timing }
    , name_{ name }
    , acronym_{ acronym }
    , description_{ description }
    , interval_{ interval }
    , variableInterval_{ variableInterval }
{
    // types_[ name ] = this;
} // ArchiveType

// std::shared_ptr< ArchiveType > ArchiveType::Create(
//     const std::string& name, const LocString& description
//     , ArchiveTimingType timing, const LocString& acronym
//     , TimeType interval, bool variableInterval )
// {
//     struct MakeSharedEnabler: public ArchiveType {
//         template < typename... Args >
//         MakeSharedEnabler( Args&&... args )
//             : ArchiveType( std::forward< Args >( args )... )
//         {}
//     };
//     return std::make_shared< MakeSharedEnabler >( name, description
//         , timing, acronym, interval, variableInterval );
// } // Create


// const std::unordered_map< std::string, const ArchiveType* >& ArchiveType::All()
// {
//     return types_;
// } // All


ArchiveTimingType ArchiveType::GetTimingType() const
{
    return timing_;
} // GetTimingType


const std::string& ArchiveType::GetName() const
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


std::string ArchiveType::ToString() const
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

/// @file Реализация интервального архива
/// @copyright HypeRRu 2024

#include <logika/meters/interval_archive.h>

#include <logika/common/misc.h>

/// @cond
#include <stdexcept>
#include <algorithm>
/// @endcond

namespace logika
{

namespace meters
{

IntervalArchive::IntervalArchive( std::shared_ptr< Meter > meter, std::shared_ptr< ArchiveType > archiveType )
    : Archive( meter, archiveType )
    , dataTable_{ std::make_shared< DataTable >() }
{
    if ( !archiveType || !archiveType->IsIntervalArchive() )
    {
        throw std::runtime_error{ "Wrong archive type" };
    }
    if ( !dataTable_ )
    {
        throw std::runtime_error{ "DataTable not created" };
    }
    if ( !AddDefaultColumns() )
    {
        throw std::runtime_error{ "Can't add default columns" };
    }
} // IntervalArchive( std::shared_ptr< Meter >, std::shared_ptr< ArchiveType > )


IntervalArchive::IntervalArchive( std::shared_ptr< Meter > meter
    , std::shared_ptr< ArchiveType > archiveType, std::shared_ptr< DataTable > tableTemplate )
    : IntervalArchive( meter, archiveType )
{
    if ( !tableTemplate )
    {
        throw std::runtime_error{ "Invalid data table template" };
    }
    for ( auto field: tableTemplate->GetFieldsList() )
    {
        if ( !field || StrCaseEq( field->GetName(), LOCALIZED( "tm" ) ) )
        {
            continue;
        }
        dataTable_->AddColumn( field );
    }
} // IntervalArchive( std::shared_ptr< Meter >, std::shared_ptr< ArchiveType >, std::shared_ptr< DataTable > )


bool IntervalArchive::AddDefaultColumns()
{
    if ( !dataTable_ )
    {
        return false;
    }

    const auto& fields = dataTable_->GetFieldsList();
    auto tmIter = std::find_if( fields.cbegin(), fields.cend(), []( const DataTable::FieldType& field ) {
        if ( !field )
        {
            return false;
        }
        return StrCaseEq( field->GetName(), LOCALIZED( "tm" ) );
    } );
    if ( fields.cend() != tmIter )
    {
        return true;
    }

    ChannelDef cDef{ meter_, LOCALIZED( "tm" ), 0, 1, LOCALIZED( "Timestamp" ) };
    ArchiveFieldDefSettings afDefSettings;
    afDefSettings.archiveType   = archiveType_;
    afDefSettings.name          = LOCALIZED( "tm" );
    afDefSettings.type          = DbType::Int64;
    afDefSettings.description   = LOCALIZED( "Timestamp" );
    std::shared_ptr< ArchiveFieldDef > afDef = std::make_shared< ArchiveFieldDef >( cDef, afDefSettings );
    DataTable::FieldType field = std::make_shared< ArchiveField >( afDef, 0 );
    
    dataTable_->AddColumn( field );
    return true;
} // AddDefaultColumns


std::shared_ptr< DataTable > IntervalArchive::GetDataTable()
{
    return dataTable_;
} // GetDataTable()


const std::shared_ptr< DataTable > IntervalArchive::GetDataTable() const
{
    return dataTable_;
} // GetDataTable() const

} // namespace meters

} // namespace logika

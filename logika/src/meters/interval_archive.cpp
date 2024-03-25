/// @file Реализация интервального архива
/// @copyright HypeRRu 2024

#include <logika/meters/interval_archive.h>

#include <logika/common/misc.h>

#include <stdexcept>
#include <algorithm>

namespace logika
{

namespace meters
{

IntervalArchive::IntervalArchive( std::shared_ptr< IMeter > meter, std::shared_ptr< ArchiveType > archiveType )
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
} // IntervalArchive( std::shared_ptr< IMeter >, std::shared_ptr< ArchiveType > )


IntervalArchive::IntervalArchive( std::shared_ptr< IMeter > meter
    , std::shared_ptr< ArchiveType > archiveType, std::shared_ptr< DataTable > tableTemplate )
    : IntervalArchive( meter, archiveType )
{
    if ( !tableTemplate )
    {
        throw std::runtime_error{ "Invalid data table template" };
    }
    const size_t fCount = tableTemplate->FieldCount();
    for ( auto field: tableTemplate->GetFieldsList() )
    {
        if ( !field || StrCaseEq( field->GetName(), "tm" ) )
        {
            continue;
        }
        dataTable_->AddColumn( field );
    }
} // IntervalArchive( std::shared_ptr< IMeter >, std::shared_ptr< ArchiveType >, std::shared_ptr< DataTable > )


bool IntervalArchive::AddDefaultColumns()
{
    if ( !dataTable_ )
    {
        return false;
    }

    const auto& fields = dataTable_->GetFieldsList();
    auto tmIter = std::find_if( fields.cbegin(), fields.cend(), []( const auto& field ){
        if ( !field )
        {
            return false;
        }
        return StrCaseEq( field->GetName(), "tm" );
    } );
    if ( fields.cend() != tmIter )
    {
        return true;
    }

    ChannelDef cDef{ meter_, "tm", 0, 1, "Timestamp" };
    ArchiveFieldDefSettings afDefSettings;
    afDefSettings.archiveType   = archiveType_;
    afDefSettings.name          = "tm";
    afDefSettings.type          = DbType::Int64;
    afDefSettings.description   = "Timestamp";
    ArchiveFieldDef afDef{ cDef, afDefSettings };
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

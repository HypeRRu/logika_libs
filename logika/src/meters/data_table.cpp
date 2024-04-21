/// @file Реализация таблицы данных
/// @copyright HypeRRu 2024

#include <logika/meters/data_table.hpp>

#include <logika/log/defines.h>

/// @cond
#include <stdexcept>
#include <algorithm>
/// @endcond

namespace logika
{

namespace meters
{

DataTable::DataTable( const std::vector< FieldType >& fieldList )
    : fieldList_{ fieldList }
    , data_{}
{} // DataTable


size_t DataTable::RecordCount() const
{
    return data_.size();
} // RecordCount


size_t DataTable::FieldCount() const
{
    return fieldList_.size();
} // FieldCount


const std::vector< DataTable::FieldType >& DataTable::GetFieldsList() const
{
    return fieldList_;
} // GetFieldsList


const std::vector< DataTable::RecordType >& DataTable::GetRecordsList() const
{
    return data_;
} // GetRecordsList


std::vector< DataTable::RecordType >& DataTable::GetRecordsList()
{
    return data_;
} // GetRecordsList


const DataTable::RecordType DataTable::GetRecord( size_t index ) const
{
    if ( index >= data_.size() )
    {
        return nullptr;
    }
    return data_.at( index );
} // GetRecord


const DataTable::RecordType DataTable::PopRecord()
{
    if ( data_.empty() )
    {
        return nullptr;
    }
    const RecordType record = data_.back();
    data_.pop_back();
    return record;
} // PopRecord


bool DataTable::RemoveRecord( DataTable::RecordType record )
{
    auto iter = std::find( data_.cbegin(), data_.cend(), record );
    if ( data_.cend() == iter )
    {
        return false;
    }
    data_.erase( iter );
    return true;
} // RemoveRecord( RecordType record )


bool DataTable::RemoveRecord( size_t index )
{
    if ( index >= data_.size() )
    {
        return false;
    }
    data_.erase( data_.begin() + index );
    return true;
} // RemoveRecord( size_t index )


bool DataTable::PushRecord( DataTable::RecordType record )
{
    if ( !ValidateRecord( record ) )
    {
        return false;
    }
    data_.push_back( record );
    return true;
} // PushRecord


bool DataTable::InsertRecord( DataTable::RecordType record, size_t index )
{
    if ( index > data_.size() || !ValidateRecord( record ) )
    {
        return false;
    }
    data_.insert( data_.begin() + index, record );
    return true;
} // InsertRecord


bool DataTable::ValidateRecord( DataTable::RecordType record )
{
    if ( !record )
    {
        LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Adding empty record" ) );
        return true;
    }
    if ( record->size() != fieldList_.size() )
    {
        LOG_WRITE( LOG_WARNING, LOCALIZED( "Record fields count not match table columns count: " )
            << record->size() << LOCALIZED( " vs " ) << fieldList_.size() );
        return false;
    }
    for ( size_t i = 0; i < fieldList_.size(); ++i )
    {
        FieldType field = fieldList_.at( i );
        std::shared_ptr< logika::Any > value = record->at( i );
        if ( !field || !value || value->Empty() )
        {
            continue;
        }
        /// @todo Сделать проверку жесткой после валидации программы
        switch ( field->GetElementType() )
        {
            case DbType::Byte:
            {
                if ( !value->CheckType< ByteType >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to byte type" ) );
                    // return false;
                }
                break;
            }
            case DbType::Int32:
            {
                if ( !value->CheckType< uint32_t >() && !value->CheckType< int32_t >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to int32 type" ) );
                    // return false;
                }
                break;
            }
            case DbType::Int64:
            {
                if ( !value->CheckType< uint64_t >() && !value->CheckType< int64_t >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to int64 type" ) );
                    // return false;
                }
                break;
            }
            case DbType::Single:
            {
                if ( !value->CheckType< float >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to single type" ) );
                    // return false;
                }
                break;
            }
            case DbType::Double:
            {
                if ( !value->CheckType< double >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to double type" ) );
                    // return false;
                }
                break;
            }
            case DbType::String:
            {
                if ( !value->CheckType< LocString >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to string type" ) );
                    // return false;
                }
                break;
            }
            case DbType::ByteArray:
            {
                if ( !value->CheckType< ByteVector >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to byte array type" ) );
                    // return false;
                }
                break;
            }
            case DbType::Int32Array:
            {
                if ( !value->CheckType< std::vector< uint32_t > >() && !value->CheckType< std::vector< int32_t > >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to int32[] type" ) );
                    // return false;
                }
                break;
            }
            case DbType::StringArray:
            {
                if ( !value->CheckType< std::vector< LocString > >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to string[] type" ) );
                    // return false;
                }
                break;
            }
            case DbType::ObjectArray:
            {
                if ( !value->CheckType< std::vector< std::shared_ptr< logika::Any > > >() )
                {
                    LOG_WRITE( LOG_WARNING, LOCALIZED( "Record field " ) << i
                        << LOCALIZED( " type does not match to object[] type" ) );
                    // return false;
                }
                break;
            }
            default:
                continue;
        }
    }
    return true;
} // ValidateRecord



bool DataTable::ValidateField( DataTable::FieldType field )
{
    if ( !field )
    {
        return false;
    }
    const auto iter = std::find_if( fieldList_.cbegin(), fieldList_.cend(), [ field ](
        const DataTable::FieldType& f ){
        return f && f->GetName() == field->GetName();
    } );
    return fieldList_.cend() == iter;
} // ValidateField


bool DataTable::AddColumn( DataTable::FieldType field )
{
    if ( !ValidateField( field ) )
    {
        return false;
    }

    fieldList_.push_back( field );
    for ( RecordType record: data_ )
    {
        if ( record )
        {
            record->push_back( std::make_shared< logika::Any >() );
        }
    }
    return true;
} // AddColumn


bool DataTable::InsertColumn( DataTable::FieldType field, size_t index )
{
    if ( !ValidateField( field ) )
    {
        return false;
    }

    fieldList_.insert( fieldList_.begin() + index, field );
    for ( RecordType record: data_ )
    {
        if ( record )
        {
            record->insert( record->begin() + index, std::make_shared< logika::Any >() );
        }
    }

    return true;
} // InsertColumn


bool DataTable::RemoveColumn( DataTable::FieldType field )
{
    size_t index = 0;
    if ( !GetColumnIndex( field, index ) )
    {
        return {};
    }
    return RemoveColumn( index );
} // RemoveColumn


bool DataTable::RemoveColumn( size_t fieldIndex )
{
    if ( fieldIndex >= fieldList_.size() )
    {
        return false;
    }
    fieldList_.erase( fieldList_.begin() + fieldIndex );
    for ( DataTable::RecordType record: data_ )
    {
        if ( record )
        {
            record->erase( record->begin() + fieldIndex );
        }
    }
    return true;
} // RemoveColumn


bool DataTable::GetColumnIndex( FieldType field, size_t& index )
{
    auto iter = std::find( fieldList_.cbegin(), fieldList_.cend(), field );
    if ( fieldList_.cend() == iter )
    {
        return false;
    }
    index = iter - fieldList_.cbegin();
    return true;
} // GetColumnIndex


std::vector< std::shared_ptr< logika::Any > > DataTable::GetColumn( FieldType field )
{
    size_t index = 0;
    if ( !GetColumnIndex( field, index ) )
    {
        return {};
    }
    return GetColumn( index );
} // GetColumn( FieldType )


std::vector< std::shared_ptr< logika::Any > > DataTable::GetColumn( size_t fieldIndex )
{
    if ( fieldIndex >= fieldList_.size() )
    {
        return {};
    }
    std::vector< std::shared_ptr< logika::Any > > column{};
    column.reserve( data_.size() );
    for ( auto record: data_ )
    {
        if ( !record )
        {
            continue;
        }
        column.emplace_back( record->at( fieldIndex ) );
    }
    return column;
} // GetColumn( size_t )


void DataTable::Clear()
{
    data_.clear();
} // Clear

} // namespace meters

} // namespace logika

/// @file Реализация хранилища ресурсов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_STORAGE_STORAGE_IPP
#define LOGIKA_STORAGE_STORAGE_IPP

namespace logika
{

namespace storage
{

/// Storage

template < typename IdType, typename StoredType >
bool Storage< IdType, StoredType >::GetItem( const IdType& identifier
    , std::shared_ptr< StoredType >& item )
{
    auto iter = items_.find( identifier );
    if ( items_.cend() == iter )
    {
        item = nullptr;
        return false;
    }
    item = iter->second;
    return true;
} // GetItem


template < typename IdType, typename StoredType >
bool Storage< IdType, StoredType >::AddItem( const IdType& identifier
    , const std::shared_ptr< StoredType >& item, bool force )
{
    if ( !force && items_.count( identifier ) > 0 )
    {
        return false;
    }
    items_[ identifier ] = item;
    return true;
} // AddItem


template < typename IdType, typename StoredType >
std::vector< IdType > Storage< IdType, StoredType >::GetKeys() const
{
    std::vector< IdType > keys;
    keys.reserve( items_.size() );
    for ( const auto& item: items_ )
    {
        keys.push_back( item.first );
    }
    return keys;
} // GetKeys


template < typename IdType, typename StoredType >
std::shared_ptr< Storage< IdType, StoredType > > Storage< IdType, StoredType >::Create()
{
    struct MakeSharedEnabler: public Storage< IdType, StoredType > {};

    return std::make_shared< MakeSharedEnabler >();
} // Create


/// StorageKeeper

template < typename IdType, typename StoredType >
std::shared_ptr< Storage< IdType, StoredType > > StorageKeeper::GetStorage()
{
    using StorageType = Storage< IdType, StoredType >;

    auto iter = storages_.find( std::type_index( typeid( StoredType ) ) );
    if ( storages_.cend() == iter )
    {
        return nullptr;
    }
    return std::dynamic_pointer_cast< StorageType >( iter->second );
} // GetStorage


template < typename IdType, typename StoredType >
bool StorageKeeper::CreateStorage()
{
    using StorageType = Storage< IdType, StoredType >;

    std::type_index idx = std::type_index( typeid( StoredType ) );
    if ( storages_.count( idx ) > 0 )
    {
        return false;
    }

    storages_[ idx ] = StorageType::Create();
    return true;
} // CreateStorage

} // namespace storage

} // namespace logika

#endif // LOGIKA_STORAGE_STORAGE_IPP

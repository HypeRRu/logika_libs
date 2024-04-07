/// @file Определение хранилища ресурсов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_STORAGE_STORAGE_HPP
#define LOGIKA_STORAGE_STORAGE_HPP

/// @cond
#include <unordered_map>
#include <typeindex>
#include <memory>
/// @endcond

namespace logika
{

namespace storage
{

class StorageKeeper;

/// @brief Интерфейс хранилища ресурса
class IStorage
{
public:
    virtual ~IStorage() = default;

}; // IStorage


/// @brief Хранилище ресурса (типизированное)
/// @tparam IdType Тип идентификатор элемента
/// @tparam StoredType Тип сохраняемых элементов
template < typename IdType, typename StoredType >
class Storage: public IStorage
{
public:
    /// @brief Получение элемента из хранилища
    /// @param[in] identifier Идентификатор элемента
    /// @param[out] item Элемент из хранилища
    /// @return Удалось ли получить элемент
    bool GetItem( const IdType& identifier, std::shared_ptr< StoredType >& item ) const;

    /// @brief Добавление элемента в хранилище
    /// @param[in] identifier Идентификатор элемента
    /// @param[in] item Добавляемый из хранилища
    /// @param[in] force Заменять элемент при наличии
    /// @return Удалось ли добавить элемент
    bool AddItem( const IdType& identifier, const std::shared_ptr< StoredType >& item, bool force = false );

    /// @brief Получение ключей элементов в хранилище
    /// @return Ключи элементов
    std::vector< IdType > GetKeys() const;

private:
    friend class StorageKeeper;
    Storage() = default;

    /// @brief Создание хранилиша ресурса
    /// @return Указатель на созданное хранилище
    static std::shared_ptr< Storage > Create();

private:
    std::unordered_map< IdType, std::shared_ptr< StoredType > > items_; ///< Список элементов

}; // Storage


/// @brief Объект для работы со всеми хранилищами ресурсов
class StorageKeeper
{
public:
    /// @brief Получние объекта для работы с хранилищами
    /// @return Объект для работы с хранилищами
    static StorageKeeper& Instance()
    {
        static StorageKeeper instance;
        return instance;
    } // Instance

    /// @brief Получение хранилища ресурса
    /// @tparam IdType Тип идентификатор элемента в хранилище
    /// @tparam StoredType Тип сохраняемых элементов в хранилище
    /// @return Объект хранилища ресурса. nullptr в случае отсутствия хранилища
    template < typename IdType, typename StoredType >
    const std::shared_ptr< Storage< IdType, StoredType > > GetStorage() const;

    /// @brief Получение хранилища ресурса
    /// @tparam IdType Тип идентификатор элемента в хранилище
    /// @tparam StoredType Тип сохраняемых элементов в хранилище
    /// @return Объект хранилища ресурса. nullptr в случае отсутствия хранилища
    template < typename IdType, typename StoredType >
    std::shared_ptr< Storage< IdType, StoredType > > GetStorage();

    /// @brief Создание хранилища ресурса
    /// @tparam IdType Тип идентификатор элемента в хранилище
    /// @tparam StoredType Тип сохраняемых элементов в хранилище
    /// @return Удалось ли создать хранилище
    template < typename IdType, typename StoredType >
    bool CreateStorage();

private:
    StorageKeeper() = default;

private:
    std::unordered_map< std::type_index, std::shared_ptr< IStorage > > storages_;   ///< Хранилища ресурсов

}; // class StorageKeeper

} // namespace storage

} // namespace logika

#include <logika/storage/storage.ipp>

#endif // LOGIKA_STORAGE_STORAGE_HPP

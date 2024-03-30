/// @file Определение загрузчика ресурсов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_RESOURCES_LOADER_HPP
#define LOGIKA_RESOURCES_LOADER_HPP

/// @cond
#include "google/protobuf/message.h"

#include <string>
#include <memory>
#include <type_traits>
/// @endcond

namespace logika
{

namespace resources
{

/// @brief Интерфейс загрузчика ресурсов
/// @tparam ResourceType Тип загружаемого ресурса
/// @note ResourceType должен быть потомком ::google::protobuf::Message
template < typename ResourceType
    , typename std::enable_if<
        std::is_base_of< ::google::protobuf::Message, ResourceType >::value, bool
    >::type = true >
class ILoader
{
public:
    virtual ~ILoader() = default;

    /// @brief Загрузка ресурса
    /// @param[in] path Путь до файла с ресурсом
    /// @return Загруженный ресурс. nullptr в случае, если загрузка не удалась
    virtual std::shared_ptr< ResourceType > Load( const std::string& path ) = 0;

}; // class ILoader


/// @brief Загрузчик ресурсов
/// @copydoc ILoader
template < typename ResourceType >
class Loader: public ILoader< ResourceType >
{
public:
    /// @copydoc ILoader::Load()
    std::shared_ptr< ResourceType > Load( const std::string& path ) override;

}; // class Loader

} // namespace resources

} // namespace logika

#include <logika/resources/loader.ipp>

#endif // LOGIKA_RESOURCES_LOADER_HPP

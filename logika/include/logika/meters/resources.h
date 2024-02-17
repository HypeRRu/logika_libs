/// @file Определение типов для работы с приборами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_RESOURCES_H
#define LOGIKA_METERS_RESOURCES_H

#include <logika/common/types.h>
#include <logika/common/iserializable.h>

#include <string>

namespace logika
{

namespace meters
{

/// @brief Класс для загрузки ресурсов
template < typename ResourceType >
class ResourcesLoader
{
public:
    bool Load( const std::string& filename, std::vector< ResourceType >& resources );
}; // class ResourcesLoader


/// @todo Для каждого ResourceType сделать свой парсер

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_TYPES_H

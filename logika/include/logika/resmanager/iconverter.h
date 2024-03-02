/// @file Определение интерфейса конвертера ресурсов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_RESMANAGER_ILOADER_H
#define LOGIKA_RESMANAGER_ILOADER_H

#include <string>

namespace logika
{

namespace resources
{

/// @brief Интерфейс конвертера ресурсов
/// @tparam Loaded Тип загруженного ресурса
/// @tparam Converted Тип выходного ресурса
template < typename Loaded, typename Converted >
class IConverter
{
public:
    virtual ~IConverter() = default;

    /// @todo

}; // class IConverter

} // namespace resources

} // namespace logika

#endif // LOGIKA_RESMANAGER_ILOADER_H

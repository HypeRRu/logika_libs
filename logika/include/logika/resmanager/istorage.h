/// @file Определение интерфейса хранилища ресурсов
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_RESMANAGER_ISTORAGE_H
#define LOGIKA_RESMANAGER_ISTORAGE_H

#include <logika/common/any.hpp>

namespace logika
{

namespace resources
{

/// @brief Интерфейс хранилища ресурсов
class IStorage
{
public:
    virtual ~IStorage() = default;

    virtual IStorage& Instance() = 0;

}; // class IStorage

} // namespace resources

} // namespace logika

#endif // LOGIKA_RESMANAGER_ISTORAGE_H

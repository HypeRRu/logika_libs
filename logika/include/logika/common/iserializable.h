/// @file Интерфейс сериализуемого класса
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_ISERIALIZABLE_H
#define LOGIKA_ISERIALIZABLE_H

#include <string>

namespace logika
{

class ISerializable
{
public:
    virtual ~ISerializable() = default;

    /// @brief Сериализация объекта
    /// @return Строковое представление объекта
    virtual std::string ToString() const = 0;

}; // class ISerializable

} // namespace logika

#endif // LOGIKA_ISERIALIZABLE_H

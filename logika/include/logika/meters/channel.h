/// @file Определение типа канала
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_CHANNEL_H
#define LOGIKA_METERS_CHANNEL_H

#include <logika/common/iserializable.h>

#include <string>
#include <memory>

#include <cstdint>

namespace logika
{

namespace meters
{

/// @brief Тип канала
enum class ChannelKind
{
    Undefined,  ///< Тип не определен
    Common,     ///< Заголовок записи
    Channel,    ///< Канал измерения / труба (СПТ(Г): "т", СПЕ: "к")
    Group,      ///< Группа каналов / потребитель (СПТ(Г): "п", СПЕ: "г")
    TV          ///< ТВ
}; // enum class ChannelKind


/// @todo replace with real Meter
struct Meter
{
public:
    ChannelKind GetChannelKind() const
    {
        return ChannelKind::Undefined;
    }

};


/// @brief Структура описание канала 
/// @todo Уточнить описание параметров
struct ChannelDef: public ISerializable
{
public:
    /// @brief Конструктор описания канала
    /// @param[in] m Прибор
    /// @param[in] p Префикс канала
    /// @param[in] s Начало
    /// @param[in] c Количество
    /// @param[in] d Описание канала
    ChannelDef( std::shared_ptr< Meter > m, const std::string& p
        , int32_t s , uint32_t c, const std::string& d );

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

public:
    const std::shared_ptr< Meter > meter;   ///< Прибор
    const ChannelKind kind;                 ///< Тип канала
    const std::string prefix;               ///< Префикс канала
    const int32_t start;                    ///< Начало
    const uint32_t count;                   ///< Количество
    const std::string description;          ///< Описание канала

}; // struct ChannelDef


/// @brief Структура канала
struct Channel: public ChannelDef
{
public:
    /// @brief Конструктор канала
    /// @param[in] cdef Описание канала
    /// @param[in] channelNo Номер канала
    Channel( const ChannelDef& cdef, int32_t channelNo );

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

public:
    const int32_t no;       ///< Номер канала
    const std::string name; ///< Имя канала

}; // struct Channel

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_CHANNEL_H

/// @file Определение типа тэга Logika4M
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_TAG_DEF4M_H
#define LOGIKA_METERS_LOGIKA4_4M_TAG_DEF4M_H

#include <logika/meters/logika4/tag_def4.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания тэга Logika4M
struct TagDef4MSettings: public TagDef4Settings
{
public:

}; // struct TagDef4MSettings


/// @brief Тэг с данными Logika4M
class TagDef4M: public TagDef4
{
public:
    /// @brief Конструктор тэга Logika4M
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга Logika4M
    TagDef4M( const ChannelDef& cdef, const TagDef4MSettings& settings );

}; // class TagDef4M

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_TAG_DEF4M_H
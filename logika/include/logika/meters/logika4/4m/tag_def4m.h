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
struct DataTagDef4MSettings: public DataTagDef4Settings
{
public:

}; // struct DataTagDef4MSettings


/// @brief Тэг с данными Logika4M
class DataTagDef4M: public DataTagDef4
{
public:
    /// @brief Конструктор тэга Logika4M
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга Logika4M
    DataTagDef4M( const ChannelDef& cdef, const DataTagDef4MSettings& settings );

}; // class DataTagDef4M

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_TAG_DEF4M_H
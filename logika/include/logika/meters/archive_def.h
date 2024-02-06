/// @file Определение типа описания архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ARCHIVE_DEF_H
#define LOGIKA_METERS_ARCHIVE_DEF_H

#include <logika/meters/item_def_base.h>
#include <logika/meters/types.h>

#include <logika/common/iserializable.h>

#include <string>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания базового описания архива
struct ArchiveDefSettings
{
public:
    int ordinal = 0;                        ///< Порядковый номер
    const ArchiveType* archType = nullptr;  ///< Тип архива
    DbType type = DbType::Undefined;        ///< Тип элемента в базе данных (стандартный)
    uint32_t capacity = 0;                  ///< Емкость архива
    std::string name = "";                  ///< Описание тэга
    std::string description = "";           ///< Описание тэга

}; // struct ArchiveDefSettings


/// @brief Базовое описание архива
class ArchiveDef: public ItemDefBase, public ISerializable
{
public:
    /// @brief Конструктор базового описания архива
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры базового описания архива
    ArchiveDef( const ChannelDef& cdef, const ArchiveDefSettings& settings );

    /// @brief Получение типа архива
    /// @return Тип архива
    const ArchiveType* GetArchiveType() const;

    /// @brief Получение емкости архива
    /// @return Емкость архива
    uint32_t GetCapacity() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

protected:
    const ArchiveType* archiveType_;    ///< Тип архива (один из предопределенных)
    uint32_t capacity_;                 ///< Емкость архива (количество записей)

}; // class ArchiveDef

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ARCHIVE_DEF_H

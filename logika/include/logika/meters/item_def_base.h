/// @file Определение типа базового элемента
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ITEM_DEF_BASE_H
#define LOGIKA_METERS_ITEM_DEF_BASE_H

#include <logika/meters/channel.h>
#include <logika/meters/types.h>

/// @cond
#include <string>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Базовый элемент
class ItemDefBase
{
public:
    /// @brief Конструктор базового элемента
    /// @param[in] cdef Описание канала
    /// @param[in] ordinal Порядковый номер
    /// @param[in] name Название
    /// @param[in] description Описание
    /// @param[in] type Тип элемента в базе данных
    ItemDefBase( const ChannelDef& cdef, int32_t ordinal, const LocString& name
        , const LocString& description, DbType type );

    virtual ~ItemDefBase() = default;

    /// @brief Изменение порядкового номера элемента
    /// @param[in] ordinal Порядковый номер элемента
    void SetOrdinal( int32_t ordinal );

    /// @brief Получение описания канала
    /// @return Описание канала
    const ChannelDef& GetChannelDef() const;

    /// @brief Получение прибора
    /// @return Прибор
    const std::shared_ptr< IMeter > GetMeter() const;

    /// @brief Получение порядкового номера
    /// @return Порядковый номер
    virtual int GetOrdinal() const;

    /// @brief Получение имени
    /// @return Имя
    LocString GetName() const;

    /// @brief Получение описания
    /// @return Описание
    LocString GetDescription() const;

    /// @brief Получение типа элемента в базе данных
    /// @return Тип элемента в базе данных
    DbType GetElementType() const;
    
protected:
    /// @brief Изменение имени
    /// @param[in] name Новое имя
    void SetName( const LocString& name );

protected:
    const ChannelDef channelDef_;   ///< Описание канала
    int32_t ordinal_;               ///< Порядковый номер
    LocString name_;                ///< Имя (берется из внутренней базы тэгов)
    const LocString description_;   ///< Описание
    const DbType baseType_;         ///< Тип элемента в базе данных

}; // class ItemDefBase

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ITEM_DEF_BASE_H

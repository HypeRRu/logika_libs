/// @file Определение типа базового элемента
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ITEM_DEF_BASE_H
#define LOGIKA_METERS_ITEM_DEF_BASE_H

#include <logika/meters/channel.h>
#include <logika/meters/types.h>

#include <string>

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
    ItemDefBase( const ChannelDef& cdef, int ordinal, const std::string& name
        , const std::string& description, DbType type );

    virtual ~ItemDefBase() = default;

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
    std::string GetName() const;

    /// @brief Получение описания
    /// @return Описание
    std::string GetDescription() const;

    /// @brief Получение типа элемента в базе данных
    /// @return Тип элемента в базе данных
    DbType GetElementType() const;
    
protected:
    /// @brief Изменение имени
    /// @param[in] name Новое имя
    void SetName( const std::string& name );

protected:
    const ChannelDef channelDef_;   ///< Описание канала
    int ordinal_;                   ///< Порядковый номер
    std::string name_;              ///< Имя (берется из внутренней базы тэгов)
    const std::string description_; ///< Описание
    const DbType baseType_;         ///< Тип элемента в базе данных

}; // class ItemDefBase

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ITEM_DEF_BASE_H

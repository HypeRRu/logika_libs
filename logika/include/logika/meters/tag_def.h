/// @file Определение описания тэга
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_TAG_DEF_H
#define LOGIKA_METERS_TAG_DEF_H

#include <logika/meters/defs.h>
#include <logika/meters/item_def_base.h>

#include <logika/common/iserializable.h>

/// @cond
#include <string>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Параметры для создания базового тэга
struct LOGIKA_METERS_EXPORT TagDefSettings
{
public:
    int ordinal = 0;                    ///< Порядковый номер
    LocString name = L"";               ///< Название тэга
    StdVar stdVar = StdVar::Undefined;  ///< Тип тэга (величина)
    LocString description = L"";        ///< Описание тэга
    DbType type = DbType::Undefined;    ///< Тип элемента в базе данных (стандартный)
    LocString dbType = L"";             ///< Тип элемента в базе данных (не стандартный). Если не задан, используется стандартный
    LocString displayFormat = L"";      ///< Формат отображения

}; // struct TagDefSettings


/// @brief Базовое описание тэга
class LOGIKA_METERS_EXPORT TagDef: public ItemDefBase
{
public:
    /// @brief Конструктор тэга
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры базового тэга
    TagDef( const ChannelDef& cdef, const TagDefSettings& settings );

    /// @brief Получение описываемой величины
    /// @return Тип описываемой величина
    StdVar GetStdVar() const;

    /// @brief Получение типа в базе данных
    /// @return Строковое представление типа в базе данных
    LocString GetDbTypeStr() const;

    /// @brief Получение формата отображения
    /// @return Формат отображения
    LocString GetDisplayFormat() const;

    /// @brief Получение ключа в базе данных
    /// @return Ключ элемента в базе данных
    virtual LocString GetKey() const;

protected:
    StdVar stdVar_;                 ///< Тип описываемой величины
    const LocString dbTypeStr_;     ///< Тип элемента в БД
    const LocString displayFormat_; ///< Формат отображения элемента
    LocString key_;                 ///< Ключ элемента в БД

}; // class TagDef

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_TAG_DEF_H

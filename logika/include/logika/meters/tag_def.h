/// @file Определение типа тэга
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_TAG_DEF_H
#define LOGIKA_METERS_TAG_DEF_H

#include <logika/meters/item_def_base.h>

#include <logika/common/iserializable.h>

#include <string>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания базового тэга
struct TagDefSettings
{
public:
    int ordinal = 0;                    ///< Порядковый номер
    std::string name = "";              ///< Название тэга
    StdVar stdVar = StdVar::Undefined;  ///< Тип тэга (величина)
    std::string description = "";       ///< Описание тэга
    DbType type = DbType::Undefined;    ///< Тип элемента в базе данных (стандартный)
    std::string dbType = "";            ///< Тип элемента в базе данных (не стандартный). Если не задан, используется стандартный
    std::string displayFormat = "";     ///< Формат отображения

}; // struct TagDefSettings


/// @brief Базовое описание тэга
class TagDef: public ItemDefBase
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
    std::string GetDbTypeStr() const;

    /// @brief Получение формата отображения
    /// @return Формат отображения
    std::string GetDisplayFormat() const;

    /// @brief Получение ключа в базе данных
    /// @return Ключ элемента в базе данных
    virtual std::string GetKey() const;

protected:
    StdVar stdVar_;                     ///< Тип описываемой величины
    const std::string dbTypeStr_;       ///< Тип элемента в БД
    const std::string displayFormat_;   ///< Формат отображения элемента
    std::string key_;                   ///< Ключ элемента в БД

}; // class TagDef


/// @brief Параметры для создания тэга с данными
struct DataTagDefSettings: public TagDefSettings
{
public:
    TagKind::Type kind = TagKind::Undefined;    ///< Тип тэга
    const std::string descriptionEx = "";       ///< Расширенное описание (для настроечных параметров)
    const std::string range = "";               ///< Диапазон (для настроечных параметров)
    bool isBasicParam = true;                   ///< Является ли параметр базовым
    uint32_t updateRate = 0;                    ///< Частота обновления параметра

}; // struct DataTagDefSettings


/// @brief Описание тэга с данными
class DataTagDef: public TagDef, public ISerializable
{
public:
    /// @brief Конструктор тэга с данными
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга с данными
    DataTagDef( const ChannelDef& cdef, const DataTagDefSettings& settings );
    
    /// @brief Получение типа тэга
    /// @return Тип тэга
    TagKind::Type GetKind() const;

    /// @brief Получение расширенного описания
    /// @return Расширенное описание
    std::string GetDescriptionEx() const;

    /// @brief Получение диапазона
    /// @return Диапазон
    std::string GetRange() const;

    /// @brief Получение флага базового параметра
    /// @return Является ли параметр базовым
    bool IsBasicParam() const;

    /// @brief Получение частоты обновления
    /// @return Частота обновления
    uint32_t GetUpdateRate() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

protected:
    TagKind::Type kind_;                ///< Тип тэга
    const std::string descriptionEx_;   ///< Расширенное описание (для настроечных параметров)
    const std::string range_;           ///< Диапазон (для настроечных параметров)
    bool isBasicParam_;                 ///< Является ли параметр базовым
    uint32_t updateRate_;               ///< Частота обновления параметра

}; // class DataTagDef

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_TAG_DEF_H

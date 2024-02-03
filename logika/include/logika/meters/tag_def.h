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

/// @brief Базовое описание тэга
class TagDef: public ItemDefBase
{
public:
    /// @brief Конструктор тэга
    /// @param[in] cdef Описание канала
    /// @param[in] ordinal Порядковый номер
    /// @param[in] name Название
    /// @param[in] stdVar Описываемая величина
    /// @param[in] description Описание
    /// @param[in] type Тип элемента в базе данных (стандартный)
    /// @param[in] dbType Тип элемента в базе данных (не стандартный). Если не задан, используется стандартный
    /// @param[in] displayFormat Формат отображения
    TagDef( const ChannelDef& cdef, int ordinal, const std::string& name
        , StdVar stdVar, const std::string& description, DbType type
        , const std::string& dbType = "", const std::string& displayFormat = "" );

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


/// @brief Описание тэга с данными
class DataTagDef: public TagDef, public ISerializable
{
public:
    /// @brief Конструктор тэга с данными
    /// @param[in] cdef Описание канала
    /// @param[in] ordinal Порядковый номер
    /// @param[in] name Название
    /// @param[in] stdVar Описываемая величина
    /// @param[in] description Описание
    /// @param[in] type Тип элемента в базе данных (стандартный)
    /// @param[in] dbType Тип элемента в базе данных (не стандартный). Если не задан, используется стандартный
    /// @param[in] displayFormat Формат отображения
    /// @param[in] kind Тип тэга
    /// @param[in] isBasic Является ли параметр базовым
    /// @param[in] updRate Частота обновления
    /// @param[in] descEx Расширенное описание параметра. По умолчанию не задано
    /// @param[in] range Диапазон. По умолчанию не задан
    DataTagDef( const ChannelDef& cdef, int ordinal, const std::string& name
        , StdVar stdVar, const std::string& description, DbType type
        , const std::string& dbType, const std::string& displayFormat
        , TagKind::Type kind, bool isBasic, const uint32_t updRate
        , const std::string& descEx = "", const std::string& range = "" );
    
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

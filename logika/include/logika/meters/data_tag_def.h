/// @file Определение описания тэга с данными
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_DATA_TAG_DEF_H
#define LOGIKA_METERS_DATA_TAG_DEF_H

#include <logika/meters/defs.h>
#include <logika/meters/tag_def.h>

/// @cond
#include <unordered_map>
#include <vector>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Параметры для создания тэга с данными
struct LOGIKA_METERS_EXPORT DataTagDefSettings: public TagDefSettings
{
public:
    TagKind::Type kind = TagKind::Undefined;    ///< Тип тэга
    LocString descriptionEx = LOCALIZED( "" );  ///< Расширенное описание (для настроечных параметров)
    LocString range = LOCALIZED( "" );          ///< Диапазон (для настроечных параметров)
    bool isBasicParam = true;                   ///< Является ли параметр базовым
    uint32_t updateRate = 0;                    ///< Частота обновления параметра

}; // struct DataTagDefSettings


/// @brief Описание тэга с данными
class LOGIKA_METERS_EXPORT DataTagDef: public TagDef, public ISerializable
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
    LocString GetDescriptionEx() const;

    /// @brief Получение диапазона
    /// @return Диапазон
    LocString GetRange() const;

    /// @brief Получение флага базового параметра
    /// @return Является ли параметр базовым
    bool IsBasicParam() const;

    /// @brief Получение частоты обновления
    /// @return Частота обновления
    uint32_t GetUpdateRate() const;

    /// @brief Получение индекса элемента
    /// @return Индекс элемента
    int32_t GetIndex() const;

    /// @brief Получение адреса тэга
    /// @return Адрес тэга
    LocString GetAddress() const;

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

protected:
    TagKind::Type kind_;                ///< Тип тэга
    const LocString descriptionEx_;     ///< Расширенное описание (для настроечных параметров)
    const LocString range_;             ///< Диапазон (для настроечных параметров)
    bool isBasicParam_;                 ///< Является ли параметр базовым
    uint32_t updateRate_;               ///< Частота обновления параметра
    int32_t index_;                     ///< Номер элемента в массиве (для массивов). -1 - не задан
    LocString address_;                 ///< Адрес тэга

}; // class DataTagDef


/// @brief Хранилище описаний тэгов с данными
class LOGIKA_METERS_EXPORT DataTagDefVault
{
public:
    /// @brief Конструктор хранилища описаний тэгов с данными
    DataTagDefVault( const std::vector< std::shared_ptr< DataTagDef > >& tags );

    /// @brief Получение описания тэга
    /// @param[in] channelPrefix Префикс канала
    /// @param[in] name Название тэга
    /// @return Описание тэга или nullptr, если не найдено
    const std::shared_ptr< DataTagDef > Find( const LocString& channelPrefix, const LocString& name ) const;
    
    /// @brief Получение описания тэга
    /// @param[in] channelPrefix Префикс канала
    /// @param[in] name Название тэга
    /// @return Описание тэга или nullptr, если не найдено
    std::shared_ptr< DataTagDef > Find( const LocString& channelPrefix, const LocString& name );

    /// @brief Получение списка всех описаний тэгов
    /// @return Список описаний тэгов
    const std::vector< std::shared_ptr< DataTagDef > >& All() const;

    /// @brief Очистка спписка описаний тэгов
    void Clear();

private:
    /// @brief Формирование ключа таблицы
    /// @param[in] channelPrefix Префикс канала
    /// @param[in] name Название тэга
    /// @return Ключ в таблице
    static LocString CreateKey( const LocString& channelPrefix, const LocString& name );

private:
    std::unordered_map< LocString, std::shared_ptr< DataTagDef > > dataTagsMap_;    ///< Таблица тэгов
    std::vector< std::shared_ptr< DataTagDef > > dataTags_;                         ///< Список тэгов

}; // class DataTagDefVault

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_DATA_TAG_DEF_H

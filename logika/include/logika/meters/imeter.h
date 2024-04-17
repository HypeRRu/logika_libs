/// @file Определение интерфейса прибора
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_IMETER_H
#define LOGIKA_METERS_IMETER_H

#include <logika/common/iserializable.h>
#include <logika/common/types.h>

#include <logika/meters/defs.h>
#include <logika/meters/types.h>

#include <logika/storage/storage.hpp>

/// @cond
#include <string>
#include <memory>
#include <unordered_map>
/// @endcond

namespace logika
{

namespace meters
{

class  LOGIKA_METERS_EXPORT TagDef;
class  LOGIKA_METERS_EXPORT DataTagDefVault;
class  LOGIKA_METERS_EXPORT ArchiveDef;
class  LOGIKA_METERS_EXPORT ArchiveFieldDef;
struct LOGIKA_METERS_EXPORT ChannelDef;
class  LOGIKA_METERS_EXPORT DataTag;

/// @brief Интерфейс прибора
class LOGIKA_METERS_EXPORT IMeter: public ISerializable
{
public:
    virtual ~IMeter() = default;

    /// @brief Инициализация объекта прибора
    /// @details Загрузка описаний тэгов, архивов и полей архивов из хранилища
    /// @param[in] sKeeper Хранилище параметров
    /// @note Должен вызываться после загрузки в хранилище всех параметров
    /// @todo Возможно, сделать Lazy Load
    /// @todo Сделать static загрузку?
    virtual void Init( const storage::StorageKeeper& sKeeper ) = 0;

    /// @brief Получение типа измерения
    /// @return Тип измерения
    virtual MeasureKind GetMeasureKind() const = 0;

    /// @brief Получение названия прибора
    /// @return Название прибора
    virtual LocString GetCaption() const = 0;

    /// @brief Получение описания прибора
    /// @return Описание прибора
    virtual LocString GetDescription() const = 0;

    /// @brief Получение максимального количества каналов
    /// @return Максимальное количество каналов
    virtual uint32_t GetMaxChannels() const = 0;

    /// @brief Получение максимального количества групп
    /// @return Максимальное количество групп
    virtual uint32_t GetMaxGroups() const = 0;

    /// @brief Получение типа канала
    /// @return Тип канала
    virtual ChannelKind GetChannelKind( int32_t cStart, uint32_t cCount, const LocString& cName ) const = 0;

    /// @brief Получение краткого названия производителя
    /// @return Краткое название производителя
    virtual LocString GetVendorId() const = 0;

    /// @brief Получение полного названия производителя
    /// @return Полное название производителя
    virtual LocString GetVendor() const = 0;

    /// @brief Получение типа протокола
    /// @return Тип протокола
    virtual BusProtocolType GetBusType() const = 0;

    /// @brief Получение префикса события тепловычислителя (ТВ)
    /// @param[in] tv Номер события тепловычислителя
    /// @return Префикс события тепловычислителя
    virtual LocString GetEventPrefix( uint32_t tv ) const = 0;

    /// @brief Получение хранилища описаний тэгов
    /// @return Хранилище описаний тэгов
    virtual const std::shared_ptr< DataTagDefVault > GetTagsVault() const = 0;

    /// @brief Получение списка описаний полей архивов
    /// @return Список описаний полей архивов
    virtual const std::vector< std::shared_ptr< ArchiveFieldDef > >& GetArchiveFields() const = 0;

    /// @brief Получение списка описаний архивов
    /// @return Список описаний архивов
    virtual const std::vector< std::shared_ptr< ArchiveDef > >& GetArchives() const = 0;

    /// @brief Получение списка описаний каналов
    /// @return Список описаний каналов
    virtual const std::vector< std::shared_ptr< ChannelDef > >& GetChannels() const = 0;

    /// @brief Получение формата отображения
    /// @param[in] def Описание тэга
    /// @return Формат отображения для тэга
    virtual LocString GetDisplayFormat( std::shared_ptr< TagDef > def ) const = 0;

    /// @brief Получение идентификатора прибора
    /// @return Идентификатор прибора
    virtual uint16_t GetIdent() const = 0;

    /// @brief Проверка на совпадение идентификатора прибора
    /// @param[in] id0 Старший байт идентификатора
    /// @param[in] id1 Младший байт идентификатора
    /// @param[in] version Модификация
    /// @return Совпадает ли идентификатор прибора
    virtual bool IdentMatch( ByteType id0, ByteType id1, ByteType version ) const = 0;

    /// @brief Получение NT из значения тэга
    /// @param[in] value Значение тэга
    /// @param[out] out NT
    /// @return Удалось ли конвертировать значение
    virtual bool GetNtFromTag( const LocString& value, ByteType& out ) const = 0;

    /// @brief Получение списка имен стандартных тэгов
    /// @return Список имен стандартных тэгов
    virtual const std::unordered_map< ImportantTag, std::vector< LocString > >& GetCommonTagDefs() const = 0;

    /// @brief Формирование списка тэгов по описанию тэгов
    /// @param[in] tagDefList Список описаний тэгов
    /// @return Список тэгов
    virtual std::vector< std::shared_ptr< DataTag > > LookupCommonTags(
        const std::vector< LocString >& tagDefList ) const = 0;

    /// @brief Формирование списка стандартных тэгов
    /// @return Список стандартных тэгов
    virtual std::unordered_map<
        ImportantTag,
        std::vector< std::shared_ptr< DataTag > >
    >GetWellKnownTags() const = 0;

}; // class Meter

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_IMETER_H

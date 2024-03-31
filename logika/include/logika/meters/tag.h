/// @file Определение тэга
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_TAG_H
#define LOGIKA_METERS_TAG_H

#include <logika/meters/tag_def.h>
#include <logika/meters/channel.h>

#include <logika/common/iserializable.h>

/// @cond
#include <string>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Тэг
class Tag: public ISerializable
{
public:
    /// @brief Конструктор тэга
    /// @param[in] def Описание тэга
    /// @param[in] channelNo Номер используемого канала
    Tag( std::shared_ptr< TagDef > def, int32_t channelNo );

    /// @brief Получение описание тэга
    /// @return Описание тэга
    std::shared_ptr< TagDef > GetDef() const;

    /// @brief Получение имени тэга
    /// @return Имя тэга
    LocString GetName() const;

    /// @brief Получение имени поля
    /// @return Имя поля
    LocString GetFieldName() const;

    /// @brief Получение порядкового номера
    /// @return Порядковый номер
    int32_t GetOrdinal() const;

    /// @brief Получение описания тэга
    /// @return Описание тэга
    LocString GetDescription() const;

    /// @brief Получение адреса
    /// @return Адрес
    LocString GetAddress() const;

    /// @brief Получение единиц измерения
    /// @return Единицы измерения
    LocString GetEu() const;

    /// @brief Установка единиц измерения
    /// @param[in] eu Единицы измерения
    void SetEu( const LocString& eu );

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

protected:
    LocString address_;             ///< Адрес тэга
    std::shared_ptr< TagDef > def_; ///< Описание тэга
    Channel channel_;               ///< Канал
    LocString eu_;                  ///< Единицы измерения

}; // class Tag

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_TAG_H

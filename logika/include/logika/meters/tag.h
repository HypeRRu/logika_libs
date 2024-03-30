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
    Tag( const TagDef& def, int32_t channelNo );

    /// @brief Получение описание тэга
    /// @return Описание тэга
    const TagDef& GetDef() const;

    /// @brief Получение имени тэга
    /// @return Имя тэга
    std::string GetName() const;

    /// @brief Получение имени поля
    /// @return Имя поля
    std::string GetFieldName() const;

    /// @brief Получение порядкового номера
    /// @return Порядковый номер
    int32_t GetOrdinal() const;

    /// @brief Получение описания тэга
    /// @return Описание тэга
    std::string GetDescription() const;

    /// @brief Получение адреса
    /// @return Адрес
    const std::string& GetAddress() const;

    /// @brief Получение единиц измерения
    /// @return Единицы измерения
    const std::string& GetEu() const;

    /// @brief Установка единиц измерения
    /// @param[in] eu Единицы измерения
    void SetEu( const std::string& eu );

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;
protected:
    std::string address_;   ///< Адрес тэга
    TagDef def_;            ///< Описание тэга
    Channel channel_;       ///< Канал
    std::string eu_;        ///< Единицы измерения

}; // class Tag

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_TAG_H

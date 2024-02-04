/// @file Определение типа тэга Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_TAG_DEF4L_H
#define LOGIKA_METERS_LOGIKA4_4L_TAG_DEF4L_H

#include <logika/meters/logika4/tag_def4.h>
#include <logika/meters/logika4/4l/types.h>
#include <logika/common/types.h>

namespace logika
{

namespace meters
{

/// @brief Параметры для создания тэга Logika4L
struct DataTagDef4LSettings: public DataTagDef4Settings
{
public:
    BinaryType4L::Type intType = BinaryType4L::Undefined;   ///< Внутренний тип тэга
    bool inRam = false;                                     ///< Находится ли параметр в ОЗУ
    MeterAddressType address = 0x0;                         ///< Адрес параметра
    MeterAddressType cOffset = 0x0;                         ///< Смещение относительно канала
    MeterAddressType addonAddress = 0x0;                    ///< Адрес дополнения
    MeterAddressType cAddonOffset = 0x0;                    ///< Смещение дополнения относительно канала

}; // struct DataTagDef4LSettings


/// @brief Тэг с данными Logika4L
class DataTagDef4L: public DataTagDef4
{
public:
    /// @brief Конструктор тэга Logika4L
    /// @param[in] cdef Описание канала
    /// @param[in] settings Параметры тэга Logika4L
    DataTagDef4L( const ChannelDef& cdef, const DataTagDef4LSettings& settings );

    /// @brief Получение внутреннего типа тэга
    /// @return Внутренний тип тэга
    BinaryType4L::Type GetInternalType() const;

    /// @brief Находится ли параметр в ОЗУ прибора
    /// @return Флаг нахождения в ОЗУ
    bool IsInRam() const;

    /// @brief Получение адреса параметра
    /// @return Адрес параметра
    MeterAddressType GetAddress() const;

    /// @brief Получение смещения относительно канала
    /// @return Смещение относительно канала
    MeterAddressType GetChannelOffset() const;

    /// @brief Получение адреса дополнения
    /// @return Адрес дополнения
    MeterAddressType GetAddonAddress() const;

    /// @brief Получение смещения дополнения относительно канала
    /// @return Смещение дополнения относительно канала
    MeterAddressType GetAddonChannelOffset() const;

protected:
    BinaryType4L::Type internalType_;       ///< Внутренний тип тэга
    bool inRam_;                            ///< Находится ли параметр в ОЗУ
    MeterAddressType address_;              ///< Адрес параметра
    MeterAddressType channelOffset_;        ///< Смещение относительно канала
    MeterAddressType addonAddress_;         ///< Адрес дополнения
    MeterAddressType addonChannelOffset_;   ///< Смещение дополнения относительно канала

}; // class DataTagDef4L

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_TAG_DEF4L_H
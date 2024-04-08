/// @file Определение типа прибора SPG741
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_SPG741_H
#define LOGIKA_METERS_LOGIKA4_4L_SPG741_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/4l/logika4l.h>

/// @cond
/// @

namespace logika
{

namespace meters
{

/// @brief Класс прибора SPG741
class LOGIKA_METERS_EXPORT Spg741: public Logika4L
{
public:
    /// @details Конструктор прибора Spg741
    /// @copydoc Logika4L::Logika4L()
    Spg741(
          MeasureKind measureKind               = MeasureKind::G
        , const LocString& caption              = L"СПГ741"
        , const LocString& description          = L"корректор СПГ741"
        , uint32_t maxChannels                  = 2
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

public:
    /// @brief Получение адреса параметра из БД
    /// @param[in] paramName Имя параметра
    /// @param[in] sp Номер СП
    /// @return Адрес параметра
    static MeterAddressType GetMappedDbParamAddress( const LocString& paramName, uint8_t sp );

    /// @brief Получение номера параметра
    /// @details Возвращает номер параметра по описанию протокола прибора (100-198).
    /// По этим номерам в частности идет запись параметров
    /// @param[in] paramName Имя параметра
    /// @param[in] sp Номер СП
    /// @return Номер параметра или -1, если не найден
    static int32_t GetMappedDbParamOrdinal( const LocString& paramName, uint8_t sp );

public:
    /// @copydoc Logika4L::GetModelFromImage()
    virtual LocString GetModelFromImage( const ByteVector& flashImage ) const override;

    /// @copydoc Logika4L::GetAdsFileLayout()
    virtual std::vector< Logika4L::AdsFlashRun > GetAdsFileLayout( bool all, const LocString& model ) const override;

public:
    static const std::vector< LocString > sensors;                      ///< Список сенсоров прибора
    static const std::vector< std::vector< LocString > > spParamMap;    ///< Карта параметров СП
    static const std::vector< LocString > sensorVars;                   ///< Параметры сенсоров

}; // class Spg741

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_SPG741_H

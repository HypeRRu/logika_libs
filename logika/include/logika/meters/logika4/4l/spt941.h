/// @file Определение типа прибора SPT941
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_SPT941_H
#define LOGIKA_METERS_LOGIKA4_4L_SPT941_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/4l/logika4l.h>

namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT941
class LOGIKA_METERS_EXPORT Spt941: public Logika4L
{
public:
    /// @details Конструктор прибора Spt941
    /// @copydoc Logika4L::Logika4L()
    Spt941(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = LOCALIZED( "СПТ941" )
        , const LocString& description          = LOCALIZED( "тепловычислитель СПТ941, мод. 01 - 08" )
        , uint32_t maxChannels                  = 3
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

public:
    /// @brief Нормализация записи часового архива
    /// @param[in] hourRec Запись часового архива
    /// @return Нормализованная запись часового архива
    static std::vector< float > ExpandHourRecord( const ByteVector& hourRec );

public:
    /// @copydoc Logika4L::GetAdsFileLayout()
    virtual std::vector< Logika4L::AdsFlashRun > GetAdsFileLayout( bool all, const LocString& model ) const override;

}; // class Spt941

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_SPT941_H

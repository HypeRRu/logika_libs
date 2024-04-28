/// @file Определение типа прибора SPT940
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_SPT940_H
#define LOGIKA_METERS_LOGIKA4_4M_SPT940_H

#include <logika/meters/logika4/4m/defs.h>
#include <logika/meters/logika4/4m/logika4m.h>


namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT940
class LOGIKA_METERS_4M_EXPORT Spt940: public Logika4M
{
public:
    /// @details Конструктор прибора Spt940
    /// @copydoc Logika4M::Logika4M()
    Spt940(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = LOCALIZED( "СПТ940" )
        , const LocString& description          = LOCALIZED( "тепловычислитель СПТ940" )
        , uint32_t maxChannels                  = 3
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

    /// @copydoc Logika4M::GetAdsTagBlocks()
    virtual std::vector< AdsTagBlock > GetAdsTagBlocks() const override;

}; // class Spt940

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_SPT940_H

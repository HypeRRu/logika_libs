/// @file Определение типа прибора SPG740
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_SPG740_H
#define LOGIKA_METERS_LOGIKA4_4M_SPG740_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/4m/logika4m.h>


namespace logika
{

namespace meters
{

/// @brief Класс прибора SPG740
class LOGIKA_METERS_EXPORT Spg740: public Logika4M
{
public:
    /// @details Конструктор прибора Spg740
    /// @copydoc Logika4M::Logika4M()
    Spg740(
          MeasureKind measureKind               = MeasureKind::G
        , const LocString& caption              = L"СПГ740"
        , const LocString& description          = L"корректор СПГ740"
        , uint32_t maxChannels                  = 3
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

    /// @copydoc Logika4M::GetAdsTagBlocks()
    virtual std::vector< AdsTagBlock > GetAdsTagBlocks() const override;

}; // class Spg740

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_SPG740_H

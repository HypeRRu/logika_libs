/// @file Определение типа прибора LGK410
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_LGK410_H
#define LOGIKA_METERS_LOGIKA4_4M_LGK410_H

#include <logika/meters/logika4/4m/defs.h>
#include <logika/meters/logika4/4m/logika4m.h>


namespace logika
{

namespace meters
{

/// @brief Класс прибора LGK410
class LOGIKA_METERS_4M_EXPORT Lgk410: public Logika4M
{
public:
    /// @details Конструктор прибора Lgk410
    /// @copydoc Logika4M::Logika4M()
    Lgk410(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = LOCALIZED( "ЛГК410" )
        , const LocString& description          = LOCALIZED( "расходомер ЛГК410" )
        , uint32_t maxChannels                  = 1
        , uint32_t maxGroups                    = 0
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

    /// @copydoc Logika4M::GetAdsTagBlocks()
    virtual std::vector< AdsTagBlock > GetAdsTagBlocks() const override;

}; // class Lgk410

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_LGK410_H

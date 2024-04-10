/// @file Определение типа прибора SPT944
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_SPT944_H
#define LOGIKA_METERS_LOGIKA4_4M_SPT944_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/4m/logika4m.h>


namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT944
class LOGIKA_METERS_EXPORT Spt944: public Logika4M
{
public:
    /// @details Конструктор прибора Spt944
    /// @copydoc Logika4M::Logika4M()
    Spt944(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = LOCALIZED( "СПТ944" )
        , const LocString& description          = LOCALIZED( "тепловычислитель СПТ944" )
        , uint32_t maxChannels                  = 6
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

    /// @copydoc Logika4M::GetAdsTagBlocks()
    virtual std::vector< AdsTagBlock > GetAdsTagBlocks() const override;

}; // class Spt944

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_SPT944_H

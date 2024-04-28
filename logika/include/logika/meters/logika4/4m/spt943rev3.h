/// @file Определение типа прибора SPT943rev3
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_SPT943_REV3_H
#define LOGIKA_METERS_LOGIKA4_4M_SPT943_REV3_H

#include <logika/meters/logika4/4m/defs.h>
#include <logika/meters/logika4/4m/logika4m.h>


namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT943rev3
class LOGIKA_METERS_4M_EXPORT Spt943rev3: public Logika4M
{
public:
    /// @details Конструктор прибора Spt943rev3
    /// @copydoc Logika4M::Logika4M()
    Spt943rev3(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = LOCALIZED( "СПТ943rev3" )
        , const LocString& description          = LOCALIZED( "тепловычислитель СПТ943 vXX03XX" )
        , uint32_t maxChannels                  = 6
        , uint32_t maxGroups                    = 2
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc IMeter::IdentMatch()
    virtual bool IdentMatch( ByteType id0, ByteType id1, ByteType version ) const override;

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

    /// @copydoc Logika4M::GetAdsTagBlocks()
    virtual std::vector< AdsTagBlock > GetAdsTagBlocks() const override;

}; // class Spt943rev3

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_SPT943_REV3_H

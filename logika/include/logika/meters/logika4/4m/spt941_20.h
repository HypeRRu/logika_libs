/// @file Определение типа прибора SPT941_20
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_SPT941_20_H
#define LOGIKA_METERS_LOGIKA4_4M_SPT941_20_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/4m/logika4m.h>


namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT941_20
class LOGIKA_METERS_EXPORT Spt941_20: public Logika4M
{
public:
    /// @details Конструктор прибора Spt941_20
    /// @copydoc Logika4M::Logika4M()
    Spt941_20(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = L"СПТ941.20"
        , const LocString& description          = L"тепловычислитель СПТ941, мод. 20"
        , uint32_t maxChannels                  = 3
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc IMeter::IdentMatch()
    virtual bool IdentMatch( ByteType id0, ByteType id1, ByteType version ) const override;

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

    /// @copydoc Logika4M::GetAdsTagBlocks()
    virtual std::vector< AdsTagBlock > GetAdsTagBlocks() const override;

}; // class Spt941_20

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_SPT941_20_H

/// @file Определение типа прибора SPT942
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_SPT942_H
#define LOGIKA_METERS_LOGIKA4_4L_SPT942_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/4l/logika4l.h>

namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT942
class LOGIKA_METERS_EXPORT Spt942: public Logika4L
{
public:
    /// @details Конструктор прибора Spt942
    /// @copydoc Logika4L::Logika4L()
    Spt942(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = L"СПТ942"
        , const LocString& description          = L"тепловычислитель СПТ942"
        , uint32_t maxChannels                  = 6
        , uint32_t maxGroups                    = 2
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc IMeter::Init()
    virtual void Init( const storage::StorageKeeper& sKeeper ) override;

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

public:
    /// @copydoc Logika4L::GetModelFromImage()
    virtual LocString GetModelFromImage( const ByteVector& flashImage ) const override;

    /// @copydoc Logika4L::GetAdsFileLayout()
    virtual std::vector< Logika4L::AdsFlashRun > GetAdsFileLayout( bool all, const LocString& model ) const override;

}; // class Spt942

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_SPT942_H

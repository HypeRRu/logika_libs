/// @file Определение типа прибора SPT943
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_SPT943_H
#define LOGIKA_METERS_LOGIKA4_4L_SPT943_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/4l/logika4l.h>

namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT943
class LOGIKA_METERS_EXPORT Spt943: public Logika4L
{
public:
    /// @details Конструктор прибора Spt943
    /// @copydoc Logika4L::Logika4L()
    Spt943(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = L"СПТ943"
        , const LocString& description          = L"тепловычислитель СПТ943"
        , uint32_t maxChannels                  = 6
        , uint32_t maxGroups                    = 2
        , BusProtocolType busType               = BusProtocolType::RSBus
    );

    /// @copydoc IMeter::IdentMatch()
    virtual bool IdentMatch( ByteType id0, ByteType id1, ByteType version ) const override;

    /// @copydoc Logika4::BuildEuDict()
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const override;

public:
    /// @copydoc Logika4L::GetModelFromImage()
    virtual LocString GetModelFromImage( const ByteVector& flashImage ) const override;

    /// @copydoc Logika4L::GetAdsFileLayout()
    virtual std::vector< Logika4L::AdsFlashRun > GetAdsFileLayout( bool all, const LocString& model ) const override;

}; // class Spt943

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_SPT943_H

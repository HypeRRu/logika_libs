/// @file Определение типа прибора SPT941_10
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_SPT941_10_H
#define LOGIKA_METERS_LOGIKA4_4L_SPT941_10_H

#include <logika/meters/logika4/4l/defs.h>
#include <logika/meters/logika4/4l/logika4l.h>

namespace logika
{

namespace meters
{

/// @brief Класс прибора SPT941_10
class LOGIKA_METERS_4L_EXPORT Spt941_10: public Logika4L
{
public:
    /// @details Конструктор прибора Spt941_10
    /// @copydoc Logika4L::Logika4L()
    Spt941_10(
          MeasureKind measureKind               = MeasureKind::T
        , const LocString& caption              = LOCALIZED( "СПТ941.10/11" )
        , const LocString& description          = LOCALIZED( "тепловычислитель СПТ941, мод. 10, 11" )
        , uint32_t maxChannels                  = 3
        , uint32_t maxGroups                    = 1
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

}; // class Spt941_10

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_SPT941_10_H

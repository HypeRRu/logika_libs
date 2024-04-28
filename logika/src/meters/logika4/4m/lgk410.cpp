/// @file Реализация класса прибора LGK410
/// @copyright HypeRRu 2024

#include <logika/meters/logika4/4m/lgk410.h>

#include <logika/meters/data_tag.h>
#include <logika/meters/channel.h>

namespace logika
{

namespace meters
{

Lgk410::Lgk410(
      MeasureKind measureKind
    , const LocString& caption
    , const LocString& description
    , uint32_t maxChannels
    , uint32_t maxGroups
    , BusProtocolType busType
)
    : Logika4M( measureKind, caption, description, maxChannels, maxGroups, busType )
{
    ident_ = 0x460A;
    supportBaudRateChange_ = false;
    supportedByProlog4_ = true;
    maxBaudRate_ = connections::BaudRate::Rate57600;
    sessionTimeout_ = 0; /// Нет ограничения времени жизни
    supportArchivePartitions_ = false;
    supportFlz_ = false;

    commonTagDefs_ = {
        { ImportantTag::SerialNo,   { LOCALIZED( "ОБЩ.serial" ) } },
        { ImportantTag::NetAddr,    { LOCALIZED( "ОБЩ.NT" ) } },
        { ImportantTag::Ident,      { LOCALIZED( "ОБЩ.ИД" ) } },
        { ImportantTag::ParamsCSum, { LOCALIZED( "ОБЩ.КСБД" ) } }
    };
    nsDescriptions_ = {};
} // Lgk410


std::unordered_map< LocString, LocString > Lgk410::BuildEuDict(
    const std::vector< std::shared_ptr< DataTag > >& euTags ) const
{
    /// У 410 фиксированные единицы измерений -> возвращаем пустой словарь
    (void) euTags;
    return {};
} // BuildEuDict


std::vector< AdsTagBlock > Lgk410::GetAdsTagBlocks() const
{
    return {};
} // GetAdsTagBlocks

} // namespace meters

} // namespace logika

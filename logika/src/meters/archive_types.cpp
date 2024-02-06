/// @file Предопределенные типы архивов
/// @copyright HypeRRu 2024

#include <logika/meters/types.h>

#include <chrono>

namespace // anonymous
{

using namespace std::chrono;

constexpr logika::TimeType tsZero   = 0;
constexpr logika::TimeType tsMinute = duration_cast< milliseconds >( minutes( 1 ) ).count();
constexpr logika::TimeType tsHour   = duration_cast< milliseconds >( hours( 1 ) ).count();
constexpr logika::TimeType tsDay    = 24 * tsHour;
constexpr logika::TimeType tsDecade = 10 * tsDay;
constexpr logika::TimeType tsMonth  = 30 * tsDay;

} // anonymous namespace


namespace logika
{

namespace meters
{

const ArchiveType ArchiveType::Hour{
    "Hour",
    "hourly archive",
    ArchiveTimingType::Synchronous,
    "ho",
    tsHour
};
const ArchiveType ArchiveType::Day{
    "Day",
    "daily archive",
    ArchiveTimingType::Synchronous,
    "day",
    tsDay
};
const ArchiveType ArchiveType::Decade{
    "Decade",
    "ten-day archive",
    ArchiveTimingType::Synchronous,
    "dec",
    tsDecade
};
const ArchiveType ArchiveType::Month{
    "Month",
    "monthly archive",
    ArchiveTimingType::Synchronous,
    "mon",
    tsMonth
};

const ArchiveType ArchiveType::ParamsLog{
    "ParamsLog",
    "DB changes",
    ArchiveTimingType::Asynchronous,
    "chg",
    tsZero
};
const ArchiveType ArchiveType::PowerLog{
    "PowerLog",
    "power breaks",
    ArchiveTimingType::Asynchronous,
    "pwr",
    tsZero
};
const ArchiveType ArchiveType::ErrorsLog{
    "ErrorsLog",
    "abnormal",
    ArchiveTimingType::Asynchronous,
    "err",
    tsZero
};

const ArchiveType ArchiveType::Control{
    "Control",
    "control archive",
    ArchiveTimingType::Synchronous,
    "ctl",
    tsDay
};

const ArchiveType ArchiveType::Minute{
    "Minute",
    "minute archive",
    ArchiveTimingType::Synchronous,
    "min",
    tsMinute,
    true
};
const ArchiveType ArchiveType::HalfHour{
    "HalfHour",
    "half hour archive",
    ArchiveTimingType::Synchronous,
    "halhr",
    30 * tsMinute,
    true
};

const ArchiveType ArchiveType::Turn{
    "Turn",
    "turn archive",
    ArchiveTimingType::Asynchronous,
    "trn",
    tsZero
};
const ArchiveType ArchiveType::Diags{
    "Diags",
    "diagnostical archive",
    ArchiveTimingType::Asynchronous,
    "diag",
    tsZero
};

} // namespace meters

} // namespace logika

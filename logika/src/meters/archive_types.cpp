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


/// @todo Локализация
namespace logika
{

namespace meters
{

const ArchiveType ArchiveType::Hour{
    "Hour",
    L"hourly archive",
    ArchiveTimingType::Synchronous,
    L"ho",
    tsHour
};
const ArchiveType ArchiveType::Day{
    "Day",
    L"daily archive",
    ArchiveTimingType::Synchronous,
    L"day",
    tsDay
};
const ArchiveType ArchiveType::Decade{
    "Decade",
    L"ten-day archive",
    ArchiveTimingType::Synchronous,
    L"dec",
    tsDecade
};
const ArchiveType ArchiveType::Month{
    "Month",
    L"monthly archive",
    ArchiveTimingType::Synchronous,
    L"mon",
    tsMonth
};

const ArchiveType ArchiveType::ParamsLog{
    "ParamsLog",
    L"DB changes",
    ArchiveTimingType::Asynchronous,
    L"chg",
    tsZero
};
const ArchiveType ArchiveType::PowerLog{
    "PowerLog",
    L"power breaks",
    ArchiveTimingType::Asynchronous,
    L"pwr",
    tsZero
};
const ArchiveType ArchiveType::ErrorsLog{
    "ErrorsLog",
    L"abnormal",
    ArchiveTimingType::Asynchronous,
    L"err",
    tsZero
};

const ArchiveType ArchiveType::Control{
    "Control",
    L"control archive",
    ArchiveTimingType::Synchronous,
    L"ctl",
    tsDay
};

const ArchiveType ArchiveType::Minute{
    "Minute",
    L"minute archive",
    ArchiveTimingType::Synchronous,
    L"min",
    tsMinute,
    true
};
const ArchiveType ArchiveType::HalfHour{
    "HalfHour",
    L"[half]hour archive",
    ArchiveTimingType::Synchronous,
    L"halhr",
    30 * tsMinute,
    true
};

const ArchiveType ArchiveType::Turn{
    "Turn",
    L"turn archive",
    ArchiveTimingType::Asynchronous,
    L"trn",
    tsZero
};
const ArchiveType ArchiveType::Diags{
    "Diags",
    L"diagnostical archive",
    ArchiveTimingType::Asynchronous,
    L"diag",
    tsZero
};

} // namespace meters

} // namespace logika

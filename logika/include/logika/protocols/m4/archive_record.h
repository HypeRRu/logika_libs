/// @file Объявление структуры записи архива в протоколе M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_ARCHIVE_RECORD_H
#define LOGIKA_PROTOCOLS_M4_ARCHIVE_RECORD_H

#include <logika/protocols/defs.h>

#include <logika/common/iserializable.h>
#include <logika/common/types.h>
#include <logika/common/any.hpp>

/// @cond
#include <vector>
#include <memory>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{

/// @brief Структура записи архива в протоколе M4
struct LOGIKA_PROTOCOLS_EXPORT ArchiveRecord: public ISerializable
{
public:
    /// @brief Конструктор по умолчанию
    ArchiveRecord();

    /// @brief Конструктор с параметрами
    /// @param[in] sinceStart Метка времени (мс) с момента запуска
    /// @param[in] full Метка времени (мс) без учета момента запуска
    /// @param[in] vals Элементы записи архива
    ArchiveRecord( TimeType sinceStart, TimeType full,
        const std::vector< std::shared_ptr< logika::Any > >& vals );

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

public:
    TimeType timeSinceStart;                ///< Метка времени (мс) с момента запуска
    TimeType fullTime;                      ///< Метка времени (мс) без учета момента запуска
    std::vector<
        std::shared_ptr< logika::Any >
    > values;                               ///< Элементы записи архива

}; // struct ArchiveRecord

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_ARCHIVE_RECORD_H

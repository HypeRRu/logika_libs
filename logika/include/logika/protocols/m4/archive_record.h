/// @file Объявление структуры записи архива в протоколе M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_ARCHIVE_RECORD_H
#define LOGIKA_PROTOCOLS_M4_ARCHIVE_RECORD_H

#include <logika/common/iserializable.h>

#include <logika/common/types.h>

#include <vector>

namespace logika
{

namespace protocols
{

namespace M4
{

/// @brief Структура записи архива в протоколе M4
struct ArchiveRecord: public ISerializable
{
public:
    /// @brief Конструктор по умолчанию
    ArchiveRecord();

    /// @brief Конструктор с параметрами
    /// @param[in] sinceStart Временная метка с момента запуска
    /// @param[in] full Временная метка без учета момента запуска
    /// @param[in] vals Элементы записи архива
    ArchiveRecord( TimeType sinceStart, TimeType full, const std::vector< ISerializable* >& vals );

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

public:
    TimeType timeSinceStart;                ///< Временная метка с момента запуска
    TimeType fullTime;                      ///< Временная метка без учета момента запуска
    std::vector< ISerializable* > values;   ///< Элементы записи архива

}; // struct ArchiveRecord

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_ARCHIVE_RECORD_H

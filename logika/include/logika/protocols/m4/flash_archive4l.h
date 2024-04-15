/// @file Объявление структуры флэш архива Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_FLASH_ARCHIVE_4L_H
#define LOGIKA_PROTOCOLS_M4_FLASH_ARCHIVE_4L_H

#include <logika/protocols/defs.h>

#include <logika/common/types.h>

/// @cond
#include <memory>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{

struct LOGIKA_PROTOCOLS_EXPORT FrbIndex;
class  LOGIKA_PROTOCOLS_EXPORT FlashArchive4;

/// @brief Состояние чтения архива ТВ Logika4L
struct LOGIKA_PROTOCOLS_EXPORT TvReadState4L
{
public:
    int64_t index = -1;                                     ///< Указатель на текущий индекс
    int64_t restartPoint = -1;                              ///< Точка перезапуска (индекс)
    std::vector< FrbIndex > indices{};                      ///< Массив индексов элементов архива
    bool headersRead = false;                               ///< Был ли прочитан заголовок
    std::vector < FrbIndex > dirtyIndices{};                ///< Обновленные, но не записанные индексы
    uint32_t dirtyInitialCount = 0;                         ///< Количество обновленных индексов
    std::shared_ptr< FlashArchive4 > fArchive = nullptr;    ///< Флэш архив

}; // struct TvReadState4L

/// @brief Состояние запроса архива Logika4L
struct LOGIKA_PROTOCOLS_EXPORT ArchiveRequestState4L
{
public:
    std::vector< TvReadState4L > tvReadStates_{}; ///< Состояние чтения каналов

}; // struct ArchiveRequestState4L

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_FLASH_ARCHIVE_4L_H

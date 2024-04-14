/// @file Объявление структуры кольцевого буфера Flash
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_FLASH_RING_BUFFER_H
#define LOGIKA_PROTOCOLS_M4_FLASH_RING_BUFFER_H

#include <logika/protocols/defs.h>

#include <logika/common/types.h>

/// @cond
#include <vector>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{

/// @brief Массив Flash
class LOGIKA_PROTOCOLS_EXPORT FlashArray
{
public:

public:
    static const MeterAddressType PAGE_SIZE;    ///< Размер страницы Flash-памяти

protected:
    ByteVector flash_;                          ///< Буфер флэш памяти
    std::vector< bool > pageMap_;               ///< Использованные страницы
    const MeterAddressType dataAddress_;        ///< Адрес начала данных
    const uint32_t elementsCount_;              ///< Количество записей
    const uint32_t elementSize_;                ///< Размер записи
    const uint32_t zeroPageNumber_;             ///< Номер нулевой страницы
    const MeterAddressType firstElementOffset_; ///< Отступ первого элемента
    const std::shared_ptr<
        M4Protocol::MeterCache 
    > mtrInstance_;

}; // class FlashArray

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_FLASH_RING_BUFFER_H

/// @file Объявление структуры кольцевого буфера Flash
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_FLASH_RING_BUFFER_H
#define LOGIKA_PROTOCOLS_M4_FLASH_RING_BUFFER_H

#include <logika/protocols/defs.h>

#include <logika/common/types.h>
#include <logika/common/iserializable.h>

#include <logika/protocols/m4/m4protocol.h>

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

struct LOGIKA_PROTOCOLS_EXPORT FrbIndex;

/// @brief Массив flash
class LOGIKA_PROTOCOLS_EXPORT FlashArray
{
public:
    /// @brief Конструктор массива flash
    /// @param[in] meterInstance Кэш прибора
    /// @param[in] dataAddr Адрес начала данных
    /// @param[in] elementsCount Количество элементов
    /// @param[in] elementSize Размер элемента
    FlashArray( std::shared_ptr< M4Protocol::MeterCache > meterInstance, MeterAddressType dataAddr,
        uint32_t elementsCount, MeterAddressType elementSize );

    /// @brief Получение количества элементов
    /// @return Количество элементов
    uint32_t GetElementsCount() const;

    /// @brief Получение размера элемента
    /// @return Размер элемента
    MeterAddressType GetElementSize() const;

    /// @brief Получение элемента
    /// @param[in] index Номер элемента
    /// @param[out] buffer Образ флэш памяти
    /// @param[out] offset Отступ до элемента
    void GetElement( uint32_t index, ByteVector& buffer, MeterAddressType& offset );

    /// @brief Обновление массива элементов
    /// @param[inout] indices Номера элементов
    void UpdateElements( std::vector< FrbIndex >& indices );

    /// @brief Сброс образа флэщ памяти
    virtual void Reset();

public:
    static const MeterAddressType PAGE_SIZE;    ///< Размер страницы Flash-памяти

protected:
    /// @brief Получение номера первой страницы, включающей элемент
    /// @param[in] elementIndex Номер элемента
    /// @return Номер первой страницы, включающей элемент
    uint32_t StartPage( uint32_t elementIndex );

    /// @brief Получение номера последней страницы, включающей элемент
    /// @param[in] elementIndex Номер элемента
    /// @return Номер последней страницы, включающей элемент
    uint32_t EndPage( uint32_t elementIndex );

    /// @brief Проверка доступности элемента
    /// @param[in] index Номер элемента
    /// @return Доступен ли элемент
    bool ElementAvailable( uint32_t index );

    /// @brief Обновление страниц
    /// @param[in] startPage Номер первой обновляемой страницы
    /// @param[in] endPage Номер последней обновляемой страницы
    void UpdatePages( uint32_t startPage, uint32_t endPage );

    /// @brief Обновление элемента
    /// @param[in] index Номер элемента
    void UpdateElementExplicit( uint32_t index );

    /// @brief Инвалидация элемента
    /// @param[in] index Номер элемента
    void InvalidateElement( uint32_t index );

protected:
    /// @brief Расширение диапазона страниц
    /// @param[in] page Номер страницы
    /// @param[inout] startPage Номер первой страницы
    /// @param[inout] endPage Номер последней страницы
    /// @return Удалось ли расширить диапазон страниц
    static bool ExtendPageRange( uint32_t page, int64_t& startPage, int64_t& endPage );

protected:
    const std::shared_ptr<
        M4Protocol::MeterCache
    > mtrInstance_;                             ///< Кэш прибора
    ByteVector flash_;                          ///< Буфер флэш памяти
    std::vector< bool > pageMap_;               ///< Использованные страницы
    const MeterAddressType dataAddress_;        ///< Адрес начала данных
    const uint32_t elementsCount_;              ///< Количество записей
    const MeterAddressType elementSize_;        ///< Размер записи
    uint32_t zeroPageNumber_;                   ///< Номер нулевой страницы
    MeterAddressType firstElementOffset_;       ///< Отступ первого элемента

}; // class FlashArray


/// @brief Кольцевой буфер flash
/// @todo реализовать
// class LOGIKA_PROTOCOLS_EXPORT FlashRingBuffer: public FlashArray
// {
// public:
//     /// @copydoc FlashArray::Reset()
//     virtual void Reset() override;

// }; // class FlashRingBuffer


/// @brief Индекс элемента Flash Ring Buffer
struct LOGIKA_PROTOCOLS_EXPORT FrbIndex: public ISerializable
{
public:
    /// @brief Конструктор индекса элемента FRB
    /// @param[in] idx Индекс элемента
    /// @param[in] t Метка времени (мс)
    FrbIndex( uint32_t idx, TimeType t );

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

public:
    /// @brief Сравнение индексов
    /// @param[in] lhs Левое сравниваемое значение
    /// @param[in] rhs Правое сравниваемое значение
    /// @return Отрицательное значение, если rhs > lhs, положительное, если rhs < lhs, 0 - если равны
    static int32_t CompareByIdx( const FrbIndex& lhs, const FrbIndex& rhs );

public:
    uint32_t index; ///< Индекс элемента
    TimeType time;  ///< Метка времени (мс)

}; // FrbIndex

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_FLASH_RING_BUFFER_H

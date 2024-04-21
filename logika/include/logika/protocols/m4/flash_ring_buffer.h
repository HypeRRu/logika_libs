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
#include <functional>
/// @endcond

namespace logika
{

namespace protocols
{

namespace M4
{

struct LOGIKA_PROTOCOLS_EXPORT FrbIndex;
class  LOGIKA_PROTOCOLS_EXPORT FlashArchive4;

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

    /// @brief Получение кэша прибора
    /// @return Кэш прибора
    std::shared_ptr< M4Protocol::MeterCache > GetMeterInstance() const;

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

    /// @brief Инвалидация элемента
    /// @param[in] index Номер элемента
    void InvalidateElement( uint32_t index );

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

protected:
    /// @brief Расширение диапазона страниц
    /// @param[in] page Номер страницы
    /// @param[inout] startPage Номер первой страницы
    /// @param[inout] endPage Номер последней страницы
    /// @return Удалось ли расширить диапазон страниц
    static bool ExtendPageRange( uint32_t page, int64_t& startPage, int64_t& endPage );

protected:
    std::shared_ptr<
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
class LOGIKA_PROTOCOLS_EXPORT FlashRingBuffer: public FlashArray
{
public:
    template < typename T >
    using GetObjectF = std::function< T( FlashArchive4*, const ByteVector&, MeterAddressType ) >;

    /// @brief Конструктор флэш архива Logika4L
    /// @param[in] parent Флэш архив
    /// @param[in] indexAddress Адрес индекса
    /// @param[in] dataAddress Адрес данных
    /// @param[in] elementsCount Количество элементов
    /// @param[in] elementSize Размер элемента
    /// @param[in] headerTimeGetter Функция для получения времени из заголовка
    /// @param[in] headerValueGetter Функция для получения значения из заголовка
    FlashRingBuffer( FlashArchive4* parent, MeterAddressType indexAddress, MeterAddressType dataAddress,
        uint32_t elementsCount, MeterAddressType elementSize,
        std::shared_ptr< GetObjectF< TimeType > > headerTimeGetter,
        std::shared_ptr< GetObjectF< LocString > > headerValueGetter );

    /// @copydoc FlashArray::Reset()
    virtual void Reset() override;

    /// @brief Получение адреса индекса
    /// @return Адрес индекса
    MeterAddressType GetIndexAddress() const;

    /// @brief Получение времени из заголовка элемента
    /// @param[in] index Номер элемента
    /// @return Время из заголовка
    TimeType GetTimeElement( uint32_t index );

    /// @brief Получение данные из заголовка элемента
    /// @param[in] index Номер элемента
    /// @return Данных из заголовка
    LocString GetValueElement( uint32_t index );

    /// @brief Был ли задан геттер времени из заголовка
    /// @return Задан ли геттер времени из заголовка
    bool TimeGetterSet() const;

    /// @brief Был ли задан геттер данных из заголовка
    /// @return Задан ли геттер данных из заголовка
    bool ValueGetterSet() const;

    /// @brief Получение индексов элементов из заданного диапазона
    /// @param[in] startTime Начало диапазона
    /// @param[in] endTime Конец диапазона
    /// @param[in] lastWrittenIndex Номер последнего записанного элемента
    /// @param[inout] restartPoint Точка перезапуска
    /// @param[out] indices Номера элементов из диапазона
    /// @param[out] percentCompleted Процент завершения
    /// @return Завершена ли обработка
    bool GetElementIndicesInRange( TimeType startTime, TimeType endTime, uint32_t lastWrittenIndex,
        int64_t& restartPoint, std::vector< FrbIndex >& indices, double& percentCompleted );

    /// @brief Обновление массива индексов
    /// @param[in] useIndexCache Нужно ли использовать кэш индексов
    /// @param[out] newIndices Обновленный массив индексов
    /// @param[out] currentIndex Номер текущего элемента
    void ManageOutdatedElements( bool useIndexCache, std::vector< uint32_t >& newIndices, uint32_t& currentIndex );

private:
    int64_t prevIdx_;                       ///< Указатель на последний считанный элемент
    TimeType prevIdxTimestamp_;             ///< Метка времени элемента, на который указывает prevIdx_
    TimeType prevReadDevTime_;              ///< Время (по часам прибора) последнего чтения
    FlashArchive4* parentArchive_;          ///< Архив
    MeterAddressType indexAddress_;         ///< Адрес индекса
    std::shared_ptr<
        GetObjectF< TimeType >
    > timeGetter_;                          ///< Функция получения времени из заголовка
    std::shared_ptr<
        GetObjectF< LocString >
    > valueGetter_;                          ///< Функция получения значения из заголовка

}; // class FlashRingBuffer


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

    /// @brief Сравнение индексов
    /// @param[in] lhs Левое сравниваемое значение
    /// @param[in] rhs Правое сравниваемое значение
    /// @return true - lhs < rhs, false - иначе
    static bool LessByIdx( const FrbIndex& lhs, const FrbIndex& rhs );

public:
    uint32_t index; ///< Индекс элемента
    TimeType time;  ///< Метка времени (мс)

}; // FrbIndex

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_FLASH_RING_BUFFER_H

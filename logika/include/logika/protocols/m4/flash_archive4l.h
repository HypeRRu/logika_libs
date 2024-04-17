/// @file Объявление структуры флэш архива Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_FLASH_ARCHIVE_4L_H
#define LOGIKA_PROTOCOLS_M4_FLASH_ARCHIVE_4L_H

#include <logika/protocols/defs.h>

#include <logika/meters/defs.h>
#include <logika/common/types.h>
#include <logika/protocols/m4/flash_ring_buffer.h>

/// @cond
#include <memory>
/// @endcond

namespace logika
{

namespace meters
{

class  LOGIKA_METERS_EXPORT ArchiveDef4L;
class  LOGIKA_METERS_EXPORT ArchiveFieldDef4L;
class  LOGIKA_METERS_EXPORT Logika4L;
struct LOGIKA_METERS_EXPORT VQT;

} // namespace meters

namespace protocols
{

namespace M4
{

class LOGIKA_PROTOCOLS_EXPORT FlashArchive4;

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


/// @brief Класс флэш архива Logika4L
class LOGIKA_PROTOCOLS_EXPORT FlashArchive4
{
public:
    /// @brief Конструктор флэш архива Logika4L
    /// @param[in] meterInstance Кэш прибора
    /// @param[in] arDef Описание архива
    /// @param[in] channelNo Номер канала
    /// @param[in] headerTimeGetter Функция для получения времени из заголовка
    /// @param[in] headerValueGetter Функция для получения значения из заголовка
    FlashArchive4( std::shared_ptr< M4Protocol::MeterCache > meterInstance,
        std::shared_ptr< meters::ArchiveDef4L > arDef,
        int32_t channelNo, MeterAddressType elementSize,
        std::shared_ptr< FlashRingBuffer::GetObjectF< TimeType > > headerTimeGetter,
        std::shared_ptr< FlashRingBuffer::GetObjectF< LocString > > headerValueGetter );

    /// @brief Получение кэша прибора
    /// @return Кэш прибора
    std::shared_ptr< M4Protocol::MeterCache > GetMeterInstance() const;

    /// @brief Получение прибора
    /// @return Прибор
    std::shared_ptr< meters::Logika4L > GetMeter() const;

    /// @brief Получение типа архива
    /// @return Тип архива
    std::shared_ptr< meters::ArchiveType > GetArchiveType() const;

    /// @brief Сброс заголовков
    void Reset();

    /// @brief Получение данных заголовка
    /// @param[in] index Номер записи
    /// @return Данные заголовка
    virtual std::shared_ptr< meters::VQT > GetDataPoint( uint32_t index );

    /// @brief Инвалидация данных
    /// @param[in] outdated Номера элементов для инвалидации
    virtual void InvalidateData( const std::vector< uint32_t >& outdated );

    /// @brief Обновление данных
    /// @param[inout] indicesToRead Номер элементов, которые необходимо прочитать
    virtual void UpdateData( std::vector< FrbIndex >& indicesToRead );

protected:
    std::shared_ptr<
        M4Protocol::MeterCache
    > mtrInstance_;                                         ///< Кэш прибора
    std::shared_ptr< meters::ArchiveDef4L > archiveDef_;    ///< Описание архива
    std::shared_ptr< FlashRingBuffer > headers_;            ///< Заголовки архива

}; // class FlashArchive4

/// @brief Синхронный (интервальный) архив Logika4L
class LOGIKA_PROTOCOLS_EXPORT SyncFlashArchive4 final: public FlashArchive4
{
public:
    /// @brief Конструктор синхронного архива
    /// @param[in] meterInstance Кэш прибора
    /// @param[in] archiveDef Описание архива
    /// @param[in] channelNo Номер канала
    SyncFlashArchive4( std::shared_ptr< M4Protocol::MeterCache > meterInstance,
        std::shared_ptr< meters::ArchiveDef4L > archiveDef, int32_t channelNo );

    /// @brief Получение суточного архива
    /// @return Суточных архив
    ByteType GetRd() const;

    /// @brief Получение часового архива
    /// @return Часовой архив
    ByteType GetRh() const;

    /// @copydoc FlashArchive4::GetDataPoint()
    virtual std::shared_ptr< meters::VQT > GetDataPoint( uint32_t index ) override;

    /// @copydoc FlashArchive4::InvalidateData()
    virtual void InvalidateData( const std::vector< uint32_t >& outdated ) override;

    /// @copydoc FlashArchive4::UpdateData()
    virtual void UpdateData( std::vector< FrbIndex >& indicesToRead ) override;

private:
    /// @brief Функция получения времени из заголовка записи синхронного архива
    /// @param[in] flashArchive Архив
    /// @param[in] buffer Буфер с данными
    /// @param[in] offset Отступ в буфере до данных
    /// @return Считанное время или -1, если не удалось прочитать
    static TimeType GetHeaderTime( FlashArchive4* flashArchive, const ByteVector& buffer, MeterAddressType offset );

private:
    FlashArray data_;   ///< Данные
    ByteType rd_;       ///< Суточный архив
    ByteType rh_;       ///< Часовой архив
    std::vector<
        std::shared_ptr< meters::ArchiveFieldDef4L >
    > fields_;          ///< Поля архива

}; // class SyncFlashArchive4


/// @brief Асинхронный архив Logika4L
class AsyncFlashArchive4: public FlashArchive4
{
public:
    /// @brief Конструктор асинхронного архива
    /// @param[in] meterInstance Кэш прибора
    /// @param[in] archiveDef Описание архива
    /// @param[in] channelNo Номер канала
    /// @param[in] valueGetter Функция для получения значения из заголовка
    AsyncFlashArchive4( std::shared_ptr< M4Protocol::MeterCache > meterInstance,
        std::shared_ptr< meters::ArchiveDef4L > archiveDef, int32_t channelNo,
        std::shared_ptr< FlashRingBuffer::GetObjectF< LocString > > valueGetter );

    /// @copydoc FlashArchive4::UpdateData()
    virtual void UpdateData( std::vector< FrbIndex >& indicesToRead ) override;

private:
    /// @brief Функция получения времени из заголовка записи асинхронного архива
    /// @param[in] flashArchive Архив
    /// @param[in] buffer Буфер с данными
    /// @param[in] offset Отступ в буфере до данных
    /// @return Считанное время или -1, если не удалось прочитать
    static TimeType GetAsyncRecordTime( FlashArchive4* flashArchive,
        const ByteVector& buffer, MeterAddressType offset );

}; // class AsyncFlashArchive4

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_FLASH_ARCHIVE_4L_H

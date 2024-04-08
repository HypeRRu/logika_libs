/// @file Определение типа прибора Logika4M
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_LOGIKA4M_H
#define LOGIKA_METERS_LOGIKA4_4M_LOGIKA4M_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/logika4.h>
#include <logika/meters/logika4/4m/ads_tag_block.h>

#include <logika/common/types.h>
#include <logika/common/any.hpp>
#include <logika/storage/storage.hpp>

namespace logika
{

namespace meters
{

namespace OperParamFlag
{

using Type = uint32_t;

/// @brief Флаг оперативности
enum : Type
{
    No  = 0,    ///< Не оперативный
    Yes = 1     ///< Оперативный
}; // anonymous enum

} // OperParamFlag


namespace TagId4M
{

using Type = uint8_t;

/// @brief Идентификатор тэга
enum : Type
{
    Empty       = 0x05, ///< Пустое значение
    Float       = 0x43, ///< Значение с плавающей точкой
    I32LE       = 0x41, ///< Значение 32 битного целого числа (Little Endian)
    ByteArray   = 0x04, ///< Массив байтов
    Ia5String   = 0x16, ///< Строка в кодировке CP1251
    Total       = 0x44, ///< Сумма INT32 и FLOAT чисел
    Oper        = 0x45, ///< Флаг оперативности тэга
    Ack         = 0x46, ///< ACK
    Time        = 0x47, ///< Время (4 байта: с / 256 - с - мин - час)
    Date        = 0x48, ///< Дата (4 байта: день - месяц - год - день недели)
    Timestamp   = 0x49, ///< Метка времени (8 байт)
    ParamNum    = 0x4A, ///< Номер параметра
    NsRecord    = 0x4B, ///< Битовая сборка НС/ДС
    Error       = 0x55  ///< Ошибка
}; // anonymous enum

} // namespace TagId4M


/// @brief Класс прибора Logika4M
class LOGIKA_METERS_EXPORT Logika4M: public Logika4
{
public:
    /// @details Конструктор прибора Logika4M
    /// @copydoc Logika4::Logika4()
    Logika4M(
          MeasureKind measureKind               = MeasureKind::Undefined
        , const LocString& caption              = L""
        , const LocString& description          = L""
        , uint32_t maxChannels                  = 1
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::Undefined
    );

    /// @copydoc IMeter::Init()
    virtual void Init( const storage::StorageKeeper& sKeeper ) override;

    /// @brief Получение списка тэгов ADS
    /// @return Список тэгов ADS
    virtual std::vector< AdsTagBlock > GetAdsTagBlocks() const;

public:
    static constexpr LocChar ND_STR[] = LOCALIZED( "#н/д" );    ///< Строка "Нет данных"

public:
    /// @brief Получение размера данных тэга
    /// @param[in] buffer Буфер
    /// @param[in] idx Адрес начала тэга
    /// @param[out] length Размер данных тэга
    /// @return Отступ до данных
    static MeterAddressType GetTagLength( const ByteVector& buffer, MeterAddressType idx, MeterAddressType& length );

    /// @brief Разбор тэга
    /// @param[in] buffer Буфер
    /// @param[in] idx Адрес начала тэга
    /// @param[out] readLen Количество считанных данных
    /// @return Пара: тип тэга - значение
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static std::pair< TagId4M::Type, std::unique_ptr< logika::Any > > ParseTag(
        const ByteVector& buffer, MeterAddressType idx, MeterAddressType& readLen );

    /// @brief Разбора тэга типа Empty
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return void
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static void                             ParseEmptyTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Float
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Float
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static float                            ParseFloatTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа I32LE
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа I32LE
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static uint32_t                         ParseI32LeTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа ByteArray
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа ByteArray
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static ByteVector                       ParseByteArrayTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа IA5String
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа IA5String
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static LocString                        ParseIa5StringTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Total
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Total
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static double                           ParseTotalTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Oper
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Oper
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static OperParamFlag::Type               ParseOperTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Ack
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Ack
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static ByteType                         ParseAckTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Time
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Time
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static LocString                        ParseTimeTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Date
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Date
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static LocString                        ParseDateTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Timestamp
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Timestamp
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static TimeType                         ParseTimestampTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа ParamNum
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа ParamNum
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static std::pair< ByteType, uint16_t >  ParseParamNumTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа NsRecord
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа NsRecord
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static std::vector< size_t >            ParseNsRecordTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );
    
    /// @brief Разбора тэга типа Error
    /// @param[in] buffer Буфер
    /// @param[in] payloadStart Адрес начала данных тэга
    /// @param[in] payloadLen Размер данных
    /// @return Значение типа Error
    /// @throws std::out_of_range Если idx + len + payload выходит за границу данных
    static ByteType                         ParseErrorTag(
        const ByteVector& buffer, MeterAddressType payloadStart, MeterAddressType payloadLen );

protected:
    bool supportFlz_;               ///< Наличие поддержки FLZ
    bool supportArchivePartitions_; ///< Наличие поддержки "разделов", врзникающих при пуске прибора

}; // class Logika4M

/// @todo AdsTagBlock

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_LOGIKA4M_H

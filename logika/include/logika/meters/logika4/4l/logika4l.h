/// @file Определение типа прибора Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_LOGIKA4L_H
#define LOGIKA_METERS_LOGIKA4_4L_LOGIKA4L_H

#include <logika/meters/defs.h>
#include <logika/meters/logika4/logika4.h>

#include <logika/meters/logika4/4l/types.h>

#include <logika/common/any.hpp>
#include <logika/storage/storage.hpp>

namespace logika
{

namespace meters
{

/// @brief Класс прибора Logika4L
class LOGIKA_METERS_EXPORT Logika4L: public Logika4
{
public:
    /// @details Конструктор прибора Logika4L
    /// @copydoc Logika4::Logika4()
    Logika4L(
          MeasureKind measureKind               = MeasureKind::Undefined
        , const LocString& caption              = LOCALIZED( "" )
        , const LocString& description          = LOCALIZED( "" )
        , uint32_t maxChannels                  = 1
        , uint32_t maxGroups                    = 1
        , BusProtocolType busType               = BusProtocolType::Undefined
    );

    /// @copydoc IMeter::Init()
    virtual void Init( const storage::StorageKeeper& sKeeper ) override;

public:
    static constexpr MeterAddressType FLASH_PAGE_SIZE   = 0x40;     ///< Размер страницы флэщ памяти
    static constexpr MeterAddressType PARAMS_FLASH_ADDR = 0x0200;   ///< Адрес параметров во флэш памяти

public:
    /// @brief Получение размера типа данных во флэш памяти
    /// @param[in] dataType Тип данных
    /// @return Размер типа данных в байтах
    static MeterAddressType SizeOfType( BinaryType4L::Type dataType );

    /// @brief Извлечение float-значения и конвертация его из представления для микроконтроллера
    /// @param[in] buffer Данные от прибора
    /// @param[in] offset Смещение в данных
    /// @return Извлеченное float-значение
    /// @throws std::out_of_range Если offset + 4 выходит за границы буфера
    static float ConvertMFloat( const ByteVector& buffer, MeterAddressType offset );

    /// @brief Извлечение даты из заголовка
    /// @param[in] arType Тип архива
    /// @param[in] rd Суточный архив
    /// @param[in] rh Часовой архив
    /// @param[in] buffer Буфер с данными
    /// @param[in] offset Отступ в буфере до данных
    /// @return 0, если заголовок битый или пустой, иначе метку времени (мс)
    static TimeType SyncHeaderToDateTime( std::shared_ptr< ArchiveType > arType
        , ByteType rd, ByteType rh, const ByteVector& buffer, MeterAddressType offset );

    /// @brief Преобразование LCD символов в человекочитаемую строку
    /// @param[in] buffer Буфер LCD символов
    /// @param[in] offset Смещение
    /// @param[in] length Длина данных
    /// @note Не человекочитаемые символы заменяются на пробельные
    /// @return Человекочитаемая строка
    static LocString LcdCharsToString( const ByteVector& buffer, MeterAddressType offset, MeterAddressType length );

    /// @brief Получение значения из буфера
    /// @param[in] binType Тип данных
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper @todo Флаг операции?
    /// @return Указатель на считанное значение (инкапсулированное в Any)
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static std::shared_ptr< logika::Any > GetValue( BinaryType4L::Type binType,
        const ByteVector& buffer, MeterAddressType offset, bool& oper );

    /// @brief Получение значения типа R32 из буфера
    /// @see BinaryType4L::R32
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение R32
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static float                  GetR32Value(         const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа Time из буфера
    /// @see BinaryType4L::Time
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа Time
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static LocString              GetTimeValue(        const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа Date из буфера
    /// @see BinaryType4L::Date
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа Date
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static LocString              GetDateValue(        const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа MmDd из буфера
    /// @see BinaryType4L::MmDdDate
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа MmDD
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static LocString              GetMmDdValue(        const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа BitArray8 из буфера
    /// @see BinaryType4L::BitArray8
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа BitArray8
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static std::vector< size_t >  GetBitArray8Value(   const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа BitArray16 из буфера
    /// @see BinaryType4L::BitArray16
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа BitArray16
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static std::vector< size_t >  GetBitArray16Value(  const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа BitArray24 из буфера
    /// @see BinaryType4L::BitArray24
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа BitArray24
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static std::vector< size_t >  GetBitArray24Value(  const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа BitArray32 из буфера
    /// @see BinaryType4L::BitArray32
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа BitArray32
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static std::vector< size_t >  GetBitArray32Value(  const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа DbEntry из буфера
    /// @see BinaryType4L::DbEntry
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа DbEntry
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static LocString              GetDbEntryValue(     const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа DbEntryByte из буфера
    /// @see BinaryType4L::DbEntryByte
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа DbEntryByte
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static ByteType               GetDbEntryByteValue( const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа U8 из буфера
    /// @see BinaryType4L::U8
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа U8
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static ByteType               GetU8Value(          const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа TripleR32 из буфера
    /// @see BinaryType4L::TripleR32
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа TripleR32
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static double                 GetTripleR32Value(   const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа I32R32 из буфера
    /// @see BinaryType4L::I32R32
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа I32R32
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static double                 GetI32R32Value(      const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа MmHh из буфера
    /// @see BinaryType4L::MmHhTime
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа MmHh
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static LocString              GetMmHhValue(        const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа U24 из буфера
    /// @see BinaryType4L::U24
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа U24
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static uint32_t               GetU24Value(         const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа ModelChar из буфера
    /// @see BinaryType4L::ModelChar
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа ModelChar
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static ByteType               GetModelCharValue(   const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа ServiceRecordTimestamp из буфера
    /// @see BinaryType4L::ServiceRecordTimestamp
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа ServiceRecordTimestamp
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static TimeType               GetSRTimestampValue( const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа NsRecord из буфера
    /// @see BinaryType4L::NsRecord
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа NsRecord
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static LocString              GetNsRecordValue(    const ByteVector& buffer, MeterAddressType offset, bool& oper );
    
    /// @brief Получение значения типа IzmRecord из буфера
    /// @see BinaryType4L::IzmRecord
    /// @param[in] buffer Буфер
    /// @param[in] offset Отступ в буфере
    /// @param[out] oper Флаг операции
    /// @return Значение типа IzmRecord
    /// @throws std::out_of_range Если offset + sizeof выходит за границу данных
    static LocString              GetIzmRecordValue(   const ByteVector& buffer, MeterAddressType offset, bool& oper );

public:
    /// @brief ADS
    struct AdsFlashRun
    {
    public:
        MeterAddressType start;     ///< Начало участка
        MeterAddressType length;    ///< Длина участка
    }; // struct AdsFlashRun

    /// @brief Получение модели прибора по образу флэш-памяти
    /// @param[in] flashImage Образ флэш-памяти прибора
    /// @return Название модели прибора
    virtual LocString GetModelFromImage( const ByteVector& flashImage ) const;

    /// @brief Получение карты ADS
    /// @param[in] all Получение полной карты
    /// @param[in] model Название модели прибора
    /// @return Карта ADS
    /// @todo Уточнить
    virtual std::vector< Logika4L::AdsFlashRun > GetAdsFileLayout( bool all, const LocString& model ) const;

}; // class Logika4L

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_LOGIKA4L_H

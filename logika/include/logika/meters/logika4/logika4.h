/// @file Определение класса прибора Logika4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_LOGIKA4_H
#define LOGIKA_METERS_LOGIKA4_LOGIKA4_H

#include <logika/meters/logika4/defs.h>
#include <logika/meters/meter.h>

#include <logika/meters/logika4/tag_def4.h>

#include <logika/common/types.h>
#include <logika/connections/serial/types.h>

#include <logika/storage/storage.hpp>

/// @cond
#include <vector>
#include <unordered_map>
#include <memory>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Класс прибора Logika4
class LOGIKA_METERS_4_EXPORT Logika4: public Meter
{
public:
    /// @details Конструктор прибора Logika4
    /// @copydoc Meter::Meter()
    Logika4(
          MeasureKind measureKind           = MeasureKind::Undefined
        , const LocString& caption          = LOCALIZED( "" )
        , const LocString& description      = LOCALIZED( "" )
        , uint32_t maxChannels              = 1
        , uint32_t maxGroups                = 1
        , BusProtocolType busType           = BusProtocolType::Undefined
    );

    /// @brief Получение описания нештатного события
    /// @param[in] index Номер нештатного события
    /// @return Описание нештатного события
    LocString GetNsDescription( uint32_t index ) const;

    /// @brief Получение описания нештатного события
    /// @param[in] ns Название НС
    /// @return Описание нештатного события
    LocString GetNsDescription( const LocString& ns ) const;

    /// @copydoc IMeter::GetDisplayFormat()
    virtual LocString GetDisplayFormat( std::shared_ptr< TagDef > def ) const override;

    /// @copydoc IMeter::GetEventPrefix()
    virtual LocString GetEventPrefix( uint32_t tv ) const override;

    /// @copydoc IMeter::GetChannelKind()
    virtual ChannelKind GetChannelKind( int32_t cStart, uint32_t cCount, const LocString& cName ) const override;

    /// @copydoc IMeter::GetNtFromTag()
    virtual bool GetNtFromTag( const LocString& value, ByteType& out ) const override;

    /// @copydoc IMeter::IdentMatch()
    virtual bool IdentMatch( ByteType id0, ByteType id1, ByteType version ) const override;

    /// @brief Получение флага наличия поддержки смены BaudRate
    /// @return Наличие поддержки смены BaudRate
    bool GetSupportBaudRateChange() const;

    /// @brief Получение максимально поддерживаемый BaudRate
    /// @return Максимальный поддерживаемый BaudRate
    connections::BaudRate::Type GetMaxBaudRate() const;

    /// @brief Получение времени жизни соединения
    /// @return Время жизни соединения (мс)
    TimeType GetSessionTimeout() const;

    /// @brief Получение флага наличия поддержки быстрой инициализации
    /// @return Наличие поддержки быстрой инициализации
    bool GetSupportFastSessionInit() const;

    /// @brief Получение описаний расчетных тэгов
    /// @return Описания расчетных тэгов
    const std::vector< std::shared_ptr< CalcFieldDef > >& GetCalculatedFields() const;

    /// @brief Формирование таблицы конвертации имен единиц измерения
    /// @param[in] euTags Список тэгов 
    /// @return Таблица конвертации имен единиц измерения
    virtual std::unordered_map< LocString, LocString > BuildEuDict(
        const std::vector< std::shared_ptr< DataTag > >& euTags ) const;

public:
    /// @brief Получение строки с номерами НС
    /// @param[in] values Битовый массив НС
    /// @return Строка с номерами НС
    static LocString DisplayNs( const ByteVector& values );

    /// @brief Получение строки с единицами измерения для давления газа
    /// @param[in] euValue Индекс единиц измерения
    /// @return Строка с единицами измерения
    static LocString GetGasPressureUnits( uint8_t euValue );

    /// @brief Определение типа прибора по идентификатору
    /// @param[in] id0 Старший байт идентификатора
    /// @param[in] id1 Младший байт идентификатора
    /// @param[in] version Модификация
    /// @param[in] meterStorage Хранилище объектов приборов
    /// @return Указатель на прибор или nullptr, если не найден
    static std::shared_ptr< Meter > DetermineMeter(
        ByteType id0, ByteType id1, ByteType version,
        std::shared_ptr< storage::Storage< LocString, Meter > > meterStorage
    );

    /// @brief Получение Timestamp (мс) из строковых представлений даты и времени
    /// @param[in] date Дата
    /// @param[in] time Время
    /// @return Метка времени (мс)
    static TimeType CombineDateTime( const LocString& date, const LocString& time );
    
    /// @brief Расчет контрольной суммы (8 бит)
    /// @param[in] buffer Данные, для которых рассчитывается контрольная сумма
    /// @param[in] start Индекс начала данных
    /// @param[in] length Длина буфера данных
    /// @return Значение контрольной суммы
    static ByteType CheckSum8( const ByteVector& buffer, uint32_t start, uint32_t length );

    /// @brief Получение описания единиц измерения
    /// @param[in] euDict Таблица конвертации имен единиц измерения
    /// @param[in] euDef Название единиц измерения
    /// @return Описание единиц измерения
    static LocString GetEu( const std::unordered_map< LocString, LocString >& euDict,
        const LocString& euDef );

protected:
    bool supportBaudRateChange_;                        ///< Наличие поддержки смены BaudRate
    connections::BaudRate::Type maxBaudRate_;           ///< Максимальный поддерживаемый BaudRate
    TimeType sessionTimeout_;                           ///< Время жизни соединения (мс)
    /// @brief Наличие поддержки быстрой инициализации
    /// @details При наличии поддержки быстрой инициализации не нужно выдерживать паузу
    /// между байтами FF и после FF-ов в стартовой последовательности
    bool supportFastSessionInit_;
    std::vector<
        std::shared_ptr< CalcFieldDef >
    > calcFields_;                                      ///< Список описаний расчетных тэгов
    std::vector< LocString > nsDescriptions_;           ///< Массив описаний нештатных событий

}; // class Logika4

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_LOGIKA4_H

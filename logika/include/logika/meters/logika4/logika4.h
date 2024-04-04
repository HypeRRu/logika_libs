/// @file Определение класса прибора Logika4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_LOGIKA4_H
#define LOGIKA_METERS_LOGIKA4_LOGIKA4_H

#include <logika/meters/meter.h>

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
class Logika4: public Meter
{
public:
    /// @details Конструктор прибора Logika4
    /// @copydoc Meter::Meter()
    Logika4(
          MeasureKind measureKind           = MeasureKind::Undefined
        , const LocString& caption          = L""
        , const LocString& description      = L""
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

    /// @brief Получение описания единиц измерения
    /// @param[in] euDef Название единиц измерения
    /// @return Описание единиц измерения
    LocString GetEu( const LocString& euDef ) const;

    /// @brief Получение флага наличия поддержки смены BaudRate
    /// @return Наличие поддержки смены BaudRate
    bool GetSupportBaudRateChange() const;

    /// @brief Получение максимально поддерживаемый BaudRate
    /// @return Максимальный поддерживаемый BaudRate
    connections::BaudRate::Type GetMaxBaudRate() const;

    /// @brief Получение времени жизни соединения
    /// @return Время жизни соединения
    TimeType GetSessionTimeout() const;

    /// @brief Получение флага наличия поддержки быстрой инициализации
    /// @return Наличие поддержки быстрой инициализации
    bool GetSupportFastSessionInit() const;

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
    
    /// @brief Расчет контрольной суммы (8 бит)
    /// @param[in] buffer Данные, для которых рассчитывается контрольная сумма
    /// @param[in] start Индекс начала данных
    /// @param[in] length Длина буфера данных
    /// @return Значение контрольной суммы
    static ByteType CheckSum8( const ByteVector& buffer, uint32_t start, uint32_t length );

protected:
    bool supportBaudRateChange_;                        ///< Наличие поддержки смены BaudRate
    connections::BaudRate::Type maxBaudRate_;           ///< Максимальный поддерживаемый BaudRate
    TimeType sessionTimeout_;                           ///< Время жизни соединения
    /// @brief Наличие поддержки быстрой инициализации
    /// @details При наличии поддержки быстрой инициализации не нужно выдерживать паузу
    /// между байтами FF и после FF-ов в стартовой последовательности
    bool supportFastSessionInit_;
    std::unordered_map< LocString, LocString > euMap_;  ///< Таблица единиц измерения
    std::vector< LocString > nsDescriptions_;           ///< Массив описаний нештатных событий

}; // class Logika4

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_LOGIKA4_H

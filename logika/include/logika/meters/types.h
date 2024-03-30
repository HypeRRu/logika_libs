/// @file Определение типов для работы с приборами
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_TYPES_H
#define LOGIKA_METERS_TYPES_H

#include <logika/common/types.h>
#include <logika/common/iserializable.h>
#include <logika/common/shared_constructible.hpp>

/// @cond
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <cstdint>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Тип измерения
enum class MeasureKind
{
    Undefined,  ///< Неопределенный тип
    T,          ///< Тепло/вода
    G,          ///< Газ
    E           ///< Электроэнергия
}; // enum class MeasureKind


/// @brief Тип протокола
enum class BusProtocolType
{
    Undefined,  ///< Неопределенный тип
    RSBus,      ///< протокол M4
    SPBus       ///< протокол СПСеть
}; // enum class BusProtocolType


/// @brief Тип канала
enum class ChannelKind
{
    Undefined,  ///< Тип не определен
    Common,     ///< Заголовок записи
    Channel,    ///< Канал измерения / труба (СПТ(Г): "т", СПЕ: "к")
    Group,      ///< Группа каналов / потребитель (СПТ(Г): "п", СПЕ: "г")
    TV          ///< ТВ
}; // enum class ChannelKind


namespace TagKind
{

using Type = uint8_t;

/// @brief Группы тэгов
enum : Type
{
    Undefined   = 0,    ///< Неизвестные
    Parameter   = 1,    ///< Настроечные
    Info        = 2,    ///< Информационные
    Realtime    = 3,    ///< Текущие
    TotalCtr    = 4     ///< Суммарные
}; // anonymous enum

}; // namespace TagKind


/// @brief Стандартные величины
enum class StdVar
{
    Undefined,  ///< Не задана
    SP,         ///< Схема потребления
    NS,         ///< Битовые сборки НС/ДС
    AVG,        ///< Среднее значение
    G,          ///< Поток
    M,          ///< Масса
    P,          ///< Давление
    dP,         ///< Изменение давления
    T,          ///< Температура
    ti,         ///< Временной интервал
    V,          ///< Объем
    W,          ///< Энергия
    auxInt      ///< int общего назначения
}; // enum class StdVar


/// @brief Тип данных элемента в БД
enum class DbType
{
    Undefined,      ///< Не задан
    Byte,           ///< Байт
    Int32,          ///< 32-битное целое число
    Int64,          ///< 64-битное целое число
    Single,         ///< 32-битное число с плавающей точкой; float(24)
    Double,         ///< 64-битное число с плавающей точкой; float(53)
    String,         ///< Строка (макс. длина - 128 байтов)
    ByteArray,      ///< Массив байтов
    Int32Array,     ///< Массив 32-битных целых чисел
    StringArray,    ///< Массив строк
    ObjectArray     ///< Массив объектов (архивный срез)
}; // enum class DbType


/// @brief Тип архива по времени измерений
enum class ArchiveTimingType
{
    Undefined,      ///< Неизвестный тип
    None,           ///< Не архив (набор тэгов)
    Synchronous,    ///< Интервальный архив
    Asynchronous    ///< Сервисный архив
}; // enum class ArchiveTimingType


/// @brief Типы тэгов
enum class ImportantTag
{
    Ident,          ///< ИД или 008
    Model,          ///< Модель (исполнение) прибора
    SerialNo,       ///< Серийный номер платы
    // ConsConfig,     ///< Конфигурация потребителей (или СП для x4)
    // PipeConfig,     ///< Конфигурация трубопроводов 
    // IfConfig,       ///< Конфигурация интерфейса(ов)
    NetAddr,        ///< Сетевой адрес
    RDay,           ///< Расчетные сутки
    RHour,          ///< Расчетный час
    EngUnits,       ///< Единицы измерения
    // TO,             ///< Время пуска
    // DO,             ///< Дата пуска
    ParamsCSum,     ///< Контрольная сумма БД, рассчитанная прибором
}; // enum class ImportantTag

namespace converters
{

class ArchiveTypeConverter;

} // namespace converters


/// @brief Тип архива
/// @todo Добавить генерацию из ресурсов
class ArchiveType: public ISerializable, public SharedConstructible< ArchiveType >
{
public:
    /// @brief Получение типа архива по времени измерений
    /// @return Тип архива по времени измерений
    ArchiveTimingType GetTimingType() const;

    /// @brief Получение название типа архива
    /// @return Название типа архива
    const std::string& GetName() const;
    
    /// @brief Получение акронима названия
    /// @return Акроним названия
    const LocString& GetAcronym() const;

    /// @brief Получение описания типа архива
    /// @return Описание типа архива
    const LocString& GetDescription() const;

    /// @brief Получение интервала архивирования
    /// @return Интервал архивирования
    TimeType GetInterval() const;

    /// @brief Получение флага переменного интервала
    /// @return Является ли интервал переменным
    bool IsVariableInterval() const;

    /// @brief Получение флага интервального архива
    /// @return Является ли архив интервальным
    bool IsIntervalArchive() const;

    /// @brief Получение флага сервисного архива
    /// @return Является ли архив сервисным (асинхронным)
    bool IsServiceArchive() const;

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

    /// @brief Конструктор типа архива
    ArchiveType( const std::string& name, const LocString& description
        , ArchiveTimingType timing, const LocString& acronym
        , TimeType interval, bool variableInterval = false );

private:
    friend class converters::ArchiveTypeConverter;

    ArchiveTimingType timing_;  ///< Тип архива по времени измерений
    std::string name_;          ///< Название типа
    LocString acronym_;         ///< Акроним названия типа
    LocString description_;     ///< Описание типа
    TimeType interval_;         ///< Интервал архивирования для интервальных архивов, мс
    bool variableInterval_;     ///< Переменный интервал

}; // class ArchiveType


/// @brief Тип, состояший из метрики, ее значения и метки времени
struct VQT: public ISerializable
{
public:
    /// @brief Конструктор без параметров
    VQT();

    /// @brief Конструктор с параметрами
    /// @param[in] v Метрика
    /// @param[in] q Значение метрики
    /// @param[in] t Метка времени
    VQT( ISerializable* v, int32_t q, TimeType t );

    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

public:
    ISerializable* value;   ///< Метрика
    int32_t quality;        ///< Значение метрики
    TimeType timestamp;     ///< Метка времени

}; // struct VQT


/// @brief Тип записей данных
struct HistoricalSeries
{
public:
    int clientHandle;           ///< Клиентский дескриптор элемента
    std::vector< VQT > data;    ///< Данные (метрики, значения, временные метки)

}; // struct HistoricalSeries


/// @brief Информация о конфигурации прибора
struct VitalInfo
{
public:
    std::string id;                         ///< Идентификатор прибора из настроечной БД
    std::string hwRev;                      ///< Модель прибора (x4) / 099н00 (x6)
    std::string hwSerial;                   ///< Серийный номер прибора
    std::vector< std::string > intfConfig;  ///< Конфигурация интерфейсов
    ByteType nt;                            ///< Байт NT
    ByteType rd;                            ///< Байт RD
    ByteType rh;                            ///< Байт RH
    // std::vector< std::string > eu;          ///< Единицы измерения (у некоторых приборов отдельно по P и Q)
    // std::string pipeConf;                   ///< Конфигурация трубопроводов (только у x6)
    // std::vector< std::string > consConf;    ///< Конфигурация потребителей (у x4 - СП, +СП по ТВ2)
    std::string mtrParamsHash;              ///< Контрольная сумма БД (рассчитанная прибором)
    TimeType clockDiff;                     ///< clockDiff = Tdevice - Tcomp.  Tdevice = Tcomp + clockDiff
}; // struct VitalInfo


/// @brief Информация о столбце БД
struct ColumnInfo: public ISerializable
{
public:
    /// @copydoc ISerializable::ToString()
    virtual std::string ToString() const override;

    bool operator ==( const ColumnInfo& other );
    bool operator !=( const ColumnInfo& other );

public:
    // int index;              ///< Индекс столбца
    std::string name;       ///< Имя столбца
    std::string dataType;   ///< Тип данных столбца
    // int maxCharLen;         ///< Максимальная длина строки
    bool nullable;          ///< Разрешено ли пустое поле

}; // struct ColumnInfo

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_TYPES_H

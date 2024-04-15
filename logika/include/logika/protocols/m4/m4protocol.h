/// @file Объявление протокола M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H
#define LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H

#include <logika/protocols/defs.h>
#include <logika/protocols/protocol.h>
#include <logika/protocols/m4/opcodes.h>
#include <logika/protocols/m4/packet.h>

#include <logika/common/types.h>
#include <logika/connections/serial/types.h>
#include <logika/meters/defs.h>
#include <logika/meters/logika4/4m/logika4m.h>

/// @cond
#include <unordered_map>
/// @endcond

namespace logika
{

namespace meters
{

/// forward declaration
class LOGIKA_METERS_EXPORT Logika4;
class LOGIKA_METERS_EXPORT Logika4L;

} // namespace meters

namespace protocols
{

namespace M4
{

struct LOGIKA_PROTOCOLS_EXPORT TagWriteData;

namespace MeterChannel
{

using Type = ByteType;

/// @brief Номер канала
enum : Type
{
    Sys = 0,    ///< Системный канал
    Tv1 = 1,    ///< 1 канал тепловычислителя
    Tv2 = 2     ///< 2 канал тепловычислителя
}; // anonymous enum

} // namespace MeterChannel

namespace RecvFlags
{

using Type = ByteType;

/// @brief Флаги чтения
enum : Type
{
    NotSet                  = 0x00, ///< Флаги не заданы
    DontThrowOnErrorReply   = 0x01  ///< Не бросать исключение при ошибке
}; // anonymous enum

} // namespace RecvFlags

/// @brief Флаги обновления тэгов
enum class TagsUpdateFlags
{
    None,       ///< Стандартное поведение
    DontGetEus  ///< Не обновлять единицы измерения
}; // enum class TagsUpdateFlags

/// @brief Протокол M4
class LOGIKA_PROTOCOLS_EXPORT M4Protocol: public Protocol
{
public:
    struct BusActiveState;

    static const ByteType FRAME_START;              ///< Начало кадра
    static const ByteType FRAME_END;                ///< Конец кадра
    static const ByteType EXT_PROTO;                ///< Флаг расширенной версии протокола
    static const ByteType BROADCAST_NT;             ///< NT для безадресных (широковещательных) запросов
    static const uint32_t MAX_RAM_REQUEST;          ///< Максимальный объем данных, запрашиваемых из RAM
    static const uint32_t MAX_TAGS_AT_ONCE;         ///< Максимальное количество считываемых за один запрос тэгов
    static const uint16_t PARTITION_CURRENT;        ///< Текущий раздел
    static const uint32_t ALT_SPEED_FALLBACK_TIME;  ///< Время сброса альтернативной скорости работы
    static const ByteVector WAKEUP_SEQUENCE;        ///< Последовательность байтов для "пробуждения" прибора
    static const TimeType WAKE_SESSION_DELAY;       ///< Задержка (мс) между серией FF и запросом сеанса (нужна только для АДС99 в режиме TCP сервер)
    static const uint32_t MAX_PAGE_BLOCK;           ///< Максимальный размер блока страниц для чтения
    static const uint32_t CHANNEL_NBASE;            ///< @todo Что это

public:
    /// @brief Конструктор шины M4
    /// @param[in] sKeeper Хранилище приложения
    /// @param[in] targetBaudRate Скорость работы с прибором
    M4Protocol( const storage::StorageKeeper& sKeeper,
        connections::BaudRate::Type targetBaudRate = connections::BaudRate::NotSupported );

    /// @brief Получение текущей скорости работы прибора
    /// @return Текущая скорость работы прибора (если прибор Serial), иначе NotSupported
    /// @throws std::logical_error если тип соединения указан как serial,
    /// но при этом соединение не является SerialConnection
    connections::BaudRate::Type GetCurrentBaudRate() const;

    /// @copydoc Protocol::ResetBusActiveState()
    virtual void ResetBusActiveState() override;

    /// @copydoc IProtocol::GetMeterType()
    virtual std::shared_ptr< meters::Meter > GetMeterType( const ByteType* srcNt,
        const ByteType* dstNt, LocString& extraData ) override;

    /// @copydoc IProtocol::UpdateTags()
    virtual void UpdateTags( const ByteType* srcNt, const ByteType* dstNt,
        std::vector< std::shared_ptr< meters::DataTag > >& tags ) override;

    /// @brief "Пробуждение" прибора
    /// @param[in] slowWake Нужно ли ожидание между байтами последовательности для пробуждения
    /// @note Совсем старым приборам нужна пауза между байтами FF
    void SendAttention( bool slowWake );

    /// @brief Определение прибора и канала
    /// @details Используется для проверки типа прибора, канала
    /// определение скорости работы и запись во внутреннее состояние шины
    /// @param[in] meter Ожидаемый тип прибора
    /// @param[in] zNt NT прибора
    /// @param[in] tv Тип канала
    void SelectDeviceAndChannel( std::shared_ptr< meters::Logika4 > meter,
        const ByteType* zNt, MeterChannel::Type tv = MeterChannel::Sys );

    /// @brief Получение пакета M4
    /// @param[in] expectedNt Ожидаемый NT пакета
    /// @param[in] expectedOpcode Ожидаемый код операции
    /// @param[in] expectedId Ожидаемый идентификатор
    /// @param[in] expectedDataLength Ожидаемая длина данных
    /// @param[in] flags Флаги получения
    /// @return Полученный пакет
    Packet RecvPacket( const ByteType* excpectedNt, Opcode::Type* expectedOpcode, const ByteType* expectedId,
        uint32_t expectedDataLength, RecvFlags::Type flags = RecvFlags::NotSet );

    /// @brief Отправка пакета (legacy)
    /// @details Отправка пакета для старых приборов Logika4L
    /// @param[in] nt NT назначения
    /// @param[in] func Код операции
    /// @param[in] data Данные пакета
    void SendLegacyPacket( const ByteType* nt, Opcode::Type func, const ByteVector& data );

    /// @brief Отправка пакета (modern)
    /// @details Отправка пакета для современных приборов Logika4M
    /// @param[in] nt NT назначения
    /// @param[in] packetId Идентификатор пакета
    /// @param[in] opcode Код операции
    /// @param[in] data Данные пакета
    void SendExtendedPacket( const ByteType* nt, ByteType packetId, Opcode::Type opcode, const ByteVector& data );

    /// @brief Отправка легаси запроса и считывание ответа
    /// @param[in] nt NT назначения
    /// @param[in] requestOpcode Код операции запроса
    /// @param[in] data Данные запроса
    /// @param[in] expectedDataLength Ожидаемая длина данных в ответе
    /// @param[in] flags Флаги запроса
    /// @return Пакет с ответом
    Packet DoLegacyRequest( const ByteType* nt, Opcode::Type requestOpcode, const ByteVector& data,
        uint32_t expectedDataLength, RecvFlags::Type flags = RecvFlags::NotSet );

    /// @brief Отправка M4 запроса и считывание ответа
    /// @param[in] nt NT назначения
    /// @param[in] requestOpcode Код операции запроса
    /// @param[in] data Данные запроса
    /// @param[in] packetId Идентификатор запроса и ответа
    /// @param[in] flags Флаги запроса
    /// @return Пакет с ответом
    Packet DoM4Request( const ByteType* nt, Opcode::Type requestOpcode, const ByteVector& data,
        const ByteType* packetId = nullptr, RecvFlags::Type flags = RecvFlags::NotSet );

    /// @brief Осуществление рукопожатия
    /// @param[in] nt NT назначения
    /// @param[in] channel Номер канала
    /// @param[in] slowWake Нужна ли пауза между байтами FF
    /// @return Пакет с ответом
    Packet DoHandshake( const ByteType* nt, ByteType channel, bool slowWake );

    /// @brief Установка скорости работы шины с прибором
    /// @param[in] meter Прибор
    /// @param[in] nt NT назначения
    /// @param[in] baudRate Устанавливаемая скорость работы
    /// @param[in] tv Номер канала
    /// @return true - удалось установить указанную скорость, false - иначе
    bool SetBusSpeed( std::shared_ptr< meters::Logika4 > meter, const ByteType* nt,
        connections::BaudRate::Type baudRate, MeterChannel::Type tv = MeterChannel::Sys );

    /// @brief Запись параметра в прибор Logika4L
    /// @param[in] meter Прибор
    /// @param[in] nt NT назначения
    /// @param[in] channel Номер канала
    /// @param[in] paramNum Номер параметра
    /// @param[in] value Значение параметра
    /// @param[in] operFlag Флаг оперативности
    /// @return Код результата операции
    Rc::Type WriteParameterL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
        ByteType channel, uint32_t paramNum, const LocString& value, bool operFlag = false );

    /// @brief Чтение данных из ОЗУ прибора Logika4L
    /// @param[in] meter Прибор
    /// @param[in] nt NT назначения
    /// @param[in] startAddr Адрес начала чтения
    /// @param[in] numBytes Количество считываемых байтов
    /// @return Данные из ОЗУ
    ByteVector ReadRamL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
        MeterAddressType startAddr, MeterAddressType numBytes );

    /// @brief Чтение страниц флэш-памяти прибора Logika4L
    /// @details Чтение страниц флэш-памяти напрямую из прибора, без использования кэша
    /// @param[in] meter Прибор
    /// @param[in] nt NT назначения
    /// @param[in] startPage Первая считываемая страница
    /// @param[in] pageCount Количество считываемых страниц
    /// @return Считанные страницы флэш-памяти
    ByteVector ReadFlashPagesL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
        uint32_t startPage, uint32_t pageCount );

    /// @brief Чтение блока данных из флэш-памяти прибора Logika4L
    /// @details Чтение блока данных флэш-памяти напрямую из прибора, без использования кэша
    /// @param[in] meter Прибор
    /// @param[in] nt NT назначения
    /// @param[in] startAddr Начало считываемого блока
    /// @param[in] length Количество считываемых данных
    /// @return Считанный блок данных из флэш-памяти
    ByteVector ReadFlashBytesL4( std::shared_ptr< meters::Logika4L > meter, const ByteType* nt,
        MeterAddressType startAddr, MeterAddressType length );
    
    /// @brief Чтение тэгов прибора Logika4M
    /// @param[in] meter Прибор
    /// @param[in] nt NT назначения
    /// @param[in] channels Каналы
    /// @param[in] ordinals Порядковые номера
    /// @param[out] opFlags Флаги оперативности
    /// @return Набор тэгов Logika4M
    std::vector< meters::Logika4M::Tag4MRecordType > ReadTags4M( std::shared_ptr< meters::Logika4M > meter,
        const ByteType* nt, const std::vector< int32_t >& channels,
        const std::vector< int32_t >& ordinals, std::vector< bool >& opFlags );

    /// @brief Запись параметров в прибор Logika4M
    /// @param[in] meter Прибор
    /// @param[in] nt NT назначения
    /// @return Ошибки записи
    std::vector< Rc::Type > WriteParams4M( std::shared_ptr< meters::Logika4M > meter, const ByteType* nt,
        const std::vector< TagWriteData >& data );

    /// @todo ReadArchive4M
    /// @todo ParseArchivePacket4M

public:
    /// @brief Генерация сырого пакет (набора байтов) рукопожатия
    /// @param[in] dstNt NT назначения
    /// @return Сырой пакет (набор байтов) рукопожатия с прибором
    static ByteVector GenerateRawHandshake( const ByteType* dstNt );

    /// @brief Добавление номера параметра в буфер
    /// @param[inout] buffer Буфер
    /// @param[in] channel Номер канала
    /// @param[in] ordinal Порядковый номер
    static void AppendParamNum( ByteVector& buffer, ByteType channel, uint16_t ordinal );

    /// @brief Разбора пакета для чтения тэгов прибора Logika4M
    /// @param[in] packet Пакет
    /// @param[out] opFlags Флаги оперативности
    static std::vector< meters::Logika4M::Tag4MRecordType > ParseM4TagsPacket( const Packet& packet,
        std::vector< bool >& opFlags );

    /// @brief Ограничение времени
    /// @details Если год в указанной дате превышает 2255, то выставляется 2255 год,
    /// т.к. год указывается одним байтом (с 2000)
    /// @param[in] date Время
    /// @return Время с ограничением
    static TimeType RestrictTime( TimeType date );

protected:
    /// @copydoc Protocol::CloseCommSessionImpl()
    virtual void CloseCommSessionImpl( const ByteType* srcNt, const ByteType* dstNt ) override;

    /// @brief Сброс скорости работы прибора до начального значения
    /// @throws std::logical_error если тип соединения указан как serial,
    /// но при этом соединение не является SerialConnection
    void SerialSpeedFallback();

    /// @brief Обработчик ошибки ввода/вывода
    /// @details Устанавливает флаг ошибки ввода вывода во внутреннем состоянии
    void OnRecoverableError();

    /// @brief Получение СП для СПГ741
    /// @param[in] nt NT назначения
    /// @return СП
    ByteType GetSpg741Sp( const ByteType* nt );

    /// @brief Заполнение даты в тэге
    /// @param[in] buffer Буфер
    /// @param[in] date Дата
    /// @param[in] useMonthYearOnly Записывать только месяц и год
    void AppendDateTag( ByteVector& buffer, TimeType date, bool useMonthYearOnly = false );

    /// @brief Реализация обновления значений тэгов
    /// @param[in] nt NT прибора
    /// @param[out] tags Обновляемые тэги
    /// @param[in] flags Флаги обновения
    void UpdateTagsImpl( const ByteType* nt, std::vector< std::shared_ptr< meters::DataTag > >& tags,
        TagsUpdateFlags flags );

private:
    connections::BaudRate::Type initialBaudRate_;   ///< Начальное значение BaudRate для соединений с автоподнятием скорости
    connections::BaudRate::Type suggestedBaudRate_; ///< Текущее значение BaudRate
    std::shared_ptr< BusActiveState > activeDev_;   ///< Внутреннее состояние шины
    static uint32_t packetId_;                      ///< Автоинкрементный идентификатор пакета

public:
    /// @brief Внутреннее состояние шины
    struct BusActiveState
    {
    public:
        /// @brief Конструктор внутреннего состояния шины
        /// @param[in] mtr Прибор
        /// @param[in] ntByte Байт nt
        /// @param[in] chan Номер канала (для Logika4L)
        BusActiveState( std::shared_ptr< meters::Logika4 > mtr, ByteType ntByte, MeterChannel::Type chan );

        /// @brief Получение времени (мс) с момента последнего ввода/вывода
        /// @return Время с момента последнего ввода/вывода (мс)
        TimeType GetTimeFromLastIo() const;

    public:
        const std::shared_ptr< meters::Logika4 > meter; ///< Активный прибор
        const ByteType nt;                              ///< Байт NT
        const MeterChannel::Type tv;                    ///< Канал (только для Logika4L)
        TimeType lastIo;                                ///< Метка времени последнего ввода/вывода (мс)
        bool hasIoError;                                ///< Наличие ошибки ввода/вывода

    }; // struct BusActiveState

public:
    /// @brief Структура записи кэша прибора
    class MeterCache
    {
    public:
        /// @brief Конструктор записи кэша прибора
        /// @param[in] owner Шина, к которой подключен прибор
        /// @param[in] meter Прибор
        /// @param[in] nt NT прибора
        MeterCache( M4Protocol* owner, std::shared_ptr< meters::Logika4 > meter, ByteType nt );

        /// @brief Получение протокола
        /// @return Протокол
        const M4Protocol* GetBus() const;

        /// @brief Получение протокола
        /// @return Протокол
        M4Protocol* GetBus();

        /// @brief Получение прибора
        /// @return Прибора
        const std::shared_ptr< meters::Logika4 > GetMeter() const;

        /// @brief Получение прибора
        /// @return Прибора
        std::shared_ptr< meters::Logika4 > GetMeter();
        
        /// @brief Получение NT прибора
        /// @return NT прибора
        ByteType GetNt() const;

        /// @brief Получение модели прибора
        /// @return Модель прибора
        LocString GetModel() const;

        /// @brief Получение СП для СПГ741
        /// @return СП
        ByteType GetSp() const;

        /// @brief Задание СП для СПГ741
        /// @param[in] sp СП
        void SetSp( ByteType sp );
        
        /// @brief Получение адреса суточного архива
        /// @return Адрес суточного архива
        MeterAddressType GetRd() const;

        /// @brief Получение адреса часового архива
        /// @return Адрес часового архива
        MeterAddressType GetRh() const;

        /// @brief Получение текущего времени на приборе
        /// @return Текущее время на приборе
        TimeType GetCurrentDeviceTime() const;

        /// @brief Получение таблицы конвертации имен единиц измерения
        /// @return Таблица конвертации имен единиц измерения
        const std::unordered_map< LocString, LocString >& GetEuDict() const;

    private:
        /// @brief Загрузка адресов часового и суточного архивов
        void LoadRdRh() const;

    private:
        M4Protocol* bus_;                               ///< Протокол
        std::shared_ptr< meters::Logika4 > meter_;      ///< Прибор
        const ByteType nt_;                             ///< NT прибора
        mutable LocString model_;                       ///< Модель прибора (использует ленивую загрузку)
        mutable bool modelSet_;                         ///< Была ли установлена модель прибора (т.к. модель может быть пустой)
        ByteType sp_;                                   ///< СП (для трансляции параметров СПГ741)
        mutable MeterAddressType rd_;                   ///< Адрес суточного архива (использует ленивую загрузку)
        mutable MeterAddressType rh_;                   ///< Адрес часового архива (использует ленивую загрузку)
        mutable std::unordered_map<
            LocString, LocString
        > euDict_;                                      ///< Таблица конвертации имен единиц измерения
        mutable TimeType deviceClockDiff_;              ///< Разница во времени между часами прибора и программы (использует ленивую загрузку)
        mutable bool clockDiffSet_;                     ///< Была ли установлена разница часов прибора и программы (т.к. у прибора может не быть часов)
        ByteVector flash_;                              ///< Образ флэш памяти (только для параметров)
        std::vector< bool > flashPageMap_;              ///< Список занятых страниц флэш памяти
        mutable std::unordered_map<
            meters::ImportantTag,
            std::vector< std::shared_ptr< meters::DataTag > >
        > vipTags_;                                     ///< Кэш стандартных тэгов (использует ленивую загрузку)

    }; // class MeterCache

}; // struct M4Protocol

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_M4PROTOCOL_H

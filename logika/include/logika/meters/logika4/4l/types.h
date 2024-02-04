/// @file Определение типов для работы с приборами Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4L_TYPES_H
#define LOGIKA_METERS_LOGIKA4_4L_TYPES_H

namespace logika
{

namespace meters
{

namespace BinaryType4L
{

using Type = uint8_t;

/// @brief Бинарный тип тэга Logika4L
enum : Type
{
    Undefined = 0,          ///< Неопределенный тип
    R32,                    ///< Одно значение с плавающей точкой (float)
    TripleR32,              ///< Три последовательных значения с плавающей точкой (float)
    Time,                   ///< Время в формате HH MM SS (3 байта)
    Date,                   ///< Дата в формате YY MM DD (3 байта)
    MmDdDate,               ///< Дата перехода на летнее/зимнее время MM-DD-xx-xx (4 байта)
    BitArray32,             ///< 32 битная сборка НС
    BitArray24,             ///< 24 битная сборка НС
    BitArray16,             ///< 16 битная сборка НС
    BitArray8,              ///< 8 битная сборка НС
    DbEntry,                ///< Тип параметра БД приборов 942, 741, 943 (строка)
    DbEntryByte,            ///< Тип параметра БД приборов (стандартный тип)
    U8,                     ///< Беззнаковое 8 битное целое число
    I32R32,                 ///< int32 + float во flash (+ float приращение за текущий час в ОЗУ, не читаем)
    MmHhTime,               ///< Время в формате MM HH
    NsRecord,               ///< Запись НС
    IzmRecord,              ///< Запись измерения
    ArchiveStruct,          ///< Архивный срез (структура, определяемая прибором)
    ModelChar,              ///< Код модели прибора
    U24,                    ///< Серийный номер прибора
    ServiceRecordTimestamp  ///< Метка времени записи сервисного архива
}; // anonymous enum

} // namespace BinaryType4L

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4L_TYPES_H

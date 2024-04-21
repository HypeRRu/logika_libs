/// @file Объявление структуры флэш архива Logika4L
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_ARCHIVE_4M_H
#define LOGIKA_PROTOCOLS_M4_ARCHIVE_4M_H

#include <logika/protocols/defs.h>

#include <logika/common/types.h>
#include <logika/meters/defs.h>

/// @cond
#include <vector>
#include <memory>
/// @endcond

namespace logika
{

namespace meters
{

class LOGIKA_METERS_EXPORT ArchiveDef4M;
class LOGIKA_METERS_EXPORT ArchiveFieldDef4M;

} // namespace meters

namespace protocols
{

namespace ArchiveId4M
{

using Type = ByteType;

/// @brief Идентификатор архива Logika4M
enum : Type
{
    Undefined   = static_cast< Type >( -1 ), ///< Не определенный тип
    Hour        = 0,    ///< Часовой архив
    Day         = 1,    ///< Суточный архив
    Dec         = 2,    ///< Декадный архив
    Mon         = 3,    ///< Месячный архив
    ParamsLog   = 4,    ///< Журнал изменений БД
    PowerLog    = 5,    ///< Журнал перерывов питания
    NsLog       = 6,    ///< Журнал НС
    Ctrl        = 7     ///< Контрольный архив
}; // anonymous enum

} // namespace ArchiveId4M

namespace CompressionType
{

using Type = ByteType;

/// @brief Тип сжатия данных
enum : Type
{
    None                = 0x00, ///< Без сжатия
    FlzLimitedLength    = 0x10, ///< FLZ compression, result length limited to ~1024 b
                                ///< в 944 реализовано с ошибкой, выдает испорченные блоки
    Flz                 = 0x20  ///< FLZ compression
}; // anonymous enum

} // namespace CompressionType

/// @brief Структура состояния запроса архива Logika4M
struct LOGIKA_PROTOCOLS_EXPORT ArchiveRequestState4M
{
public:
    std::shared_ptr<
        meters::ArchiveDef4M
    > archiveDef                    = nullptr;  ///< Описание архива
    std::vector<
        std::shared_ptr< meters::ArchiveFieldDef4M >
    > fieldsDef                     = {};       ///< Описание полей архива
    int32_t currentChannel          = -1;       ///< Номер текущего канала (для 943r3 архив приходится читать по отдельно по ТВ1 и ТВ2, потом склеивать)
    uint32_t nChannelRecordsRead    = 0;        ///< Количество считанных записей для данного канала
    TimeType timePointer            = 0;        ///< Метка времени последней записи

}; // struct ArchiveRequestState4M

/// @brief Порт реализации FastLZ
/// @details quick'n'dirty port of original FastLZ::descompress
class LOGIKA_PROTOCOLS_EXPORT FastLzImpl
{
public:
    static constexpr uint32_t MAX_DISTANCE = 8192 - 1;

    /// @brief Декомпрессия данных сжатых алгоритмом LZ77
    /// @param[in] input Входные данные
    /// @param[in] offset Отступ в данных
    /// @param[in] length Длина данных
    /// @return Расшифрованный блок данных
    static ByteVector Decompress( const ByteVector& input, uint32_t offset, uint32_t length );

}; // class FastLzImpl

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_ARCHIVE_4M_H

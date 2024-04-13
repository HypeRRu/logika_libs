/// @file Объявление структуры тэга M4
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_M4_TAG_WRITE_DATA_H
#define LOGIKA_PROTOCOLS_M4_TAG_WRITE_DATA_H

#include <logika/protocols/defs.h>
#include <logika/common/types.h>

namespace logika
{

namespace protocols
{

namespace M4
{

/// @brief Структура тэга M4
struct LOGIKA_PROTOCOLS_EXPORT TagWriteData
{
public:
    int32_t channel                      = -1;      ///< Номер канала
    int32_t ordinal                      = -1;      ///< Порядковый номер
    std::shared_ptr< logika::Any > value = nullptr; ///< Данные тэга
    bool oper                            = false;   ///< Флаг оперативности тэга

}; // struct TagWriteData

} // namespace M4

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_M4_TAG_WRITE_DATA_H

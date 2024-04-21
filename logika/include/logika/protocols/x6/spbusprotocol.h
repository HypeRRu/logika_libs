/// @file Объявление протокола SPBus
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_SPBUS_SPBUSPROTOCOL_H
#define LOGIKA_PROTOCOLS_SPBUS_SPBUSPROTOCOL_H

#include <logika/protocols/defs.h>
#include <logika/protocols/protocol.h>

#include <logika/common/types.h>

namespace logika
{

namespace protocols
{

namespace X6
{

/// @brief Протокол SPBus
class LOGIKA_PROTOCOLS_EXPORT SPBusProtocol: public Protocol
{
public:
    /// @brief Конструктор протокола SPBus
    /// @param[in] sKeeper Хранилище приложения
    /// @param[in] broadcast Использовать широковещательный режим
    SPBusProtocol( const storage::StorageKeeper& sKeeper, bool broadcast );

protected:
    bool broadcastMode_;    ///< Использовать широковещательный режим

}; // class SPBusProtocol

} // namespace X6

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_SPBUS_SPBUSPROTOCOL_H

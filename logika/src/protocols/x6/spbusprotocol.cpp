/// @file Реализация протокола SPBus
/// @copyright HypeRRu 2024

#include <logika/protocols/x6/spbusprotocol.h>

namespace logika
{

namespace protocols
{

namespace X6
{

SPBusProtocol::SPBusProtocol( const storage::StorageKeeper& sKeeper, bool broadcast )
    : Protocol( sKeeper )
    , broadcastMode_{ broadcast }
{} // SPBusProtocol

} // namespace X6

} // namespace protocols

} // namespace logika

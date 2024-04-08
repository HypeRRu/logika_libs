/// @file Определение типа тэга AdsTagBlock
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA4_4M_ADS_TAG_BLOCK_H
#define LOGIKA_METERS_LOGIKA4_4M_ADS_TAG_BLOCK_H

#include <logika/meters/defs.h>

#include <logika/common/types.h>

namespace logika
{

namespace meters
{


/// @brief Тэг ADS
class LOGIKA_METERS_EXPORT AdsTagBlock
{
public:
    /// @brief Конструктор тэга ADS
    /// @param[in] id Идентификатор тэга
    /// @param[in] channel Канал
    /// @param[in] start Начало
    /// @param[in] count Количество
    AdsTagBlock( uint32_t id, int32_t channel, uint32_t start, uint32_t count );
    
    /// @brief Конструктор тэга ADS
    /// @param[in] id Идентификатор тэга
    /// @param[in] tags Список тэгов
    AdsTagBlock( uint32_t id, const std::vector< LocString >& tags );

    /// @brief Получение идентификатора тэга
    /// @return Идентификатор тэга
    uint32_t GetId() const;

    /// @brief Получение списка каналов
    /// @return Список каналов
    const std::vector< int32_t >& GetChannels() const;

    /// @brief Получение списка номеров
    /// @return Список номеров
    const std::vector< int32_t >& GetOrdinals() const;

private:
    const uint32_t id_;                     ///< Идентификатор тэга
    std::vector< int32_t > channels_;       ///< Список каналов
    std::vector< int32_t > ordinals_;       ///< Список номеров

}; // class TagDef4M

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_LOGIKA4_4M_ADS_TAG_BLOCK_H
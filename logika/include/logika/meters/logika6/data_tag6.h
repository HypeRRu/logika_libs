/// @file Определение контейнера тэгов с данными Logika6
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_LOGIKA6_DATA_TAG6_H
#define LOGIKA_METERS_LOGIKA6_DATA_TAG6_H

#include <logika/meters/logika6/data_tag_def6.h>
#include <logika/meters/tag.h>
#include <logika/meters/data_tag.h>

/// @cond
#include <vector>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Контейнер тэгов с данными Logika6
class DataTag6Container: public Tag
{
public:
    /// @brief Конструктор контейнера тэгов
    /// @param[in] def Описание контейнера тэгов
    /// @param[in] channelNo Номер канала
    /// @param[in] leafs Описания тэгов
    DataTag6Container( std::shared_ptr< DataTagDef6 > def, int32_t channelNo
        , const std::vector< std::shared_ptr< DataTagDef6 > >& leafs );

    /// @brief Получение тэгов из контейнера
    /// @return Тэги из контейнера
    const std::vector< DataTag >& GetTags() const;

    /// @brief Получение тэгов из контейнера
    /// @return Тэги из контейнера
    std::vector< DataTag >& GetTags();

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

protected:
    std::shared_ptr< DataTagDef6 > dataTagDef_; ///< Описание тэга
    std::vector< DataTag > tags_;               ///< Массив тэгов

}; // class DataTag

} // namespace meters

} // namespace logika


#endif // LOGIKA_METERS_LOGIKA6_DATA_TAG6_H

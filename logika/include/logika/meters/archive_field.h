/// @file Определение поля архива
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_METERS_ARCHIVE_FIELD_H
#define LOGIKA_METERS_ARCHIVE_FIELD_H

#include <logika/meters/tag.h>
#include <logika/meters/archive_field_def.h>

/// @cond
#include <string>
/// @endcond

namespace logika
{

namespace meters
{

/// @brief Поле архива
class ArchiveField: public Tag
{
public:
    /// @brief Конструктор поля архива
    /// @param[in] afdef Описание поля архива
    /// @param[in] channelNo Номер используемого канала
    ArchiveField( std::shared_ptr< ArchiveFieldDef > afdef, int32_t channelNo );

    /// @brief Получение имени поля
    /// @return Имя поля, полученное от устройства
    LocString GetCaption() const;

    /// @brief Получение имени поля
    /// @param[in] Имя поля, полученное от устройства
    void SetCaption( const LocString& caption );

    /// @brief Получение номера архива
    /// @return Номер архива
    int32_t GetArchiveOrdinal() const;

    /// @brief Получение типа архива
    /// @return Тип архива
    std::shared_ptr< ArchiveType > GetArchiveType() const;

    /// @brief Получение формата отображения
    /// @return Формат отображения
    LocString GetDisplayFormat() const;

    /// @copydoc ISerializable::ToString()
    virtual LocString ToString() const override;

protected:
    LocString caption_;                             ///< Имя поля, полученное от устройства
    int32_t archiveOrd_;                            ///< Номер архива (для multipart-архивов)
    std::shared_ptr< ArchiveType > archiveType_;    ///< Тип архива

}; // class ArchiveField

} // namespace meters

} // namespace logika

#endif // LOGIKA_METERS_ARCHIVE_FIELD_H

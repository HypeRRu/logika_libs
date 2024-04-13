/// @file Описание класса ошибки обмена
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_PROTOCOLS_COMM_EXCEPTION_H
#define LOGIKA_PROTOCOLS_COMM_EXCEPTION_H

#include <logika/common/types.h>

/// @cond
#include <stdexcept>
#include <string>
/// @endcond

namespace logika
{

namespace protocols
{

/// @brief Критичность ошибки (по отношению к установленному соединению)
enum class ExceptionSeverity
{
    Error,      ///< Сброс соединения не требуется
    Reset,      ///< Требуется сброс соединения
    Stop,       ///< Требуется сброс соединения, но шина должна оставаться остановленной
    WaitRadius  ///< Соединение в порядке, но шина временно недоступна
}; // enum class ExceptionSeverity

/// @brief Причина возникновения ошибки обмена
enum class CommunicationError
{
    Timeout,        ///< Истекло время ожидания
    Checksum,       ///< Неверная контрольная сумма
    NotConnected,   ///< Соединение не установлено
    SystemError,    ///< Системная ошибка
    Unspecified     ///< Неопределенная ошибка
}; // enum class CommunicationError

/// @brief Класс ошибки обмена
class ECommException: public std::runtime_error
{
public:
    /// @brief Конструктор ошибки обмена
    /// @param[in] severity Критичность ошибки
    /// @param[in] reason Причина возникновения ошибки
    /// @param[in] baseMsg Базовое сообщение об ошибке
    /// @param[in] extInfo Расширенная информация об ошибке
    ECommException( ExceptionSeverity severity, CommunicationError reason,
        const std::string& baseMsg = "", const LocString& extInfo = LOCALIZED( "" ) );

    /// @brief Получение критичности ошибки
    /// @return Критичность ошибки
    ExceptionSeverity GetSeverity() const;

    /// @brief Получение причины ошибки
    /// @return Причина ошибки
    CommunicationError GetReason() const;

    /// @brief Получение расширенной информации об ошибки
    /// @return Расширенная информация об ошибке
    LocString GetExtendedInfo() const;

public:
    /// @brief Получение текстового представления причины ошибки
    /// @param[in] reason Причина ошибки
    /// @return Текстовое представление причины ошибки
    static LocString GetReasonDescription( CommunicationError reason );

    /// @brief Получение текстового представления критичности ошибки
    /// @param[in] severity Критичность ошибки
    /// @return Текстовое представление критичности ошибки
    static LocString GetSeverityDescription( ExceptionSeverity severity );

private:
    ExceptionSeverity severity_;    ///< Критичность ошибки
    CommunicationError reason_;     ///< Причина возникновения ошибки
    LocString extendedInfo_;        ///< Расширенная информация об ошибке

}; // class ECommException

} // namespace protocols

} // namespace logika

#endif // LOGIKA_PROTOCOLS_COMM_EXCEPTION_H

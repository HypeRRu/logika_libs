/// @file Объявление класса логгера
/// @copyright HypeRRu 2024

#pragma once

#ifndef LOGIKA_LOG_LOGGER_H
#define LOGIKA_LOG_LOGGER_H

#include <logika/common/types.h>
#include <logika/log/types.h>

#include <fstream>
#include <mutex>
#include <string>

namespace logika
{

namespace log
{

/// @brief Класс логгера
/// @note Реализует паттерн "одиночка"
class Logger
{
public:
    /// @brief Получение объекта логгера
    /// @return Объект логгера
    static Logger& Instance();

    /// @brief Задание типа логирования
    /// @param[in] type Новый тип логирования
    void SetLogType( LogType::Type type );

    /// @brief Задание пути до файла логирования
    /// @param[in] filePath Путь до файла логирования
    /// @param[in] append Открыть файл для дополнения
    void SetLogFile( const std::string& filePath, bool append = true );

    /// @brief Задание уровня логирования
    /// @param[in] level Уровень логирования
    void SetLogLevel( LogLevel::Type level );

    /// @brief Запись сообщения в лог
    /// @param[in] level Уровень логирования для записи
    /// @param[in] message Сообщение лога
    /// @param[in] funcName Имя функции для вывода в лог
    void Write( LogLevel::Type level, const LocString& message, const std::string& funcName = "" );

private:
    /// @brief Конструктор логгера
    Logger();

    /// @brief Инициализация локали
    /// @details Устанавливает системную локаль в качестве локали приложения
    /// @note Должно вызываться до первого вызова cout
    void InitLocale();

    /// @brief Проверка уровня логирования
    /// @details Печатает сообщение со всеми уровнями логирования
    void CheckLogLevel();

    Logger( const Logger& ) = delete;
    Logger& operator =( const Logger& ) = delete;
    Logger( Logger&& ) = delete;
    Logger& operator =( Logger&& ) = delete;

protected:
    LogType::Type logType_;     ///< Тип логирования
    LogLevel::Type maxLevel_;   ///< Максимальный уровень логирования
    std::wofstream fileStream_; ///< Файл логов
    std::mutex mtx_;            ///< Блокировка для синхронизации записи

}; // class Logger

} // namespace log

} // namespace logika

#endif // LOGIKA_LOG_LOGGER_H

/// @file Реализация класса ошибки обмена
/// @copyright HypeRRu 2024

#include <logika/protocols/comm_exception.h>

/// @cond
#include <unordered_map>
/// @endcond

namespace logika
{

namespace protocols
{

ECommException::ECommException( ExceptionSeverity severity, CommunicationError reason,
    const std::string& baseMsg, const LocString& extInfo )
    : std::runtime_error( baseMsg )
    , severity_{ severity }
    , reason_{ reason }
    , extendedInfo_{ extInfo }
{} // ECommException


ExceptionSeverity ECommException::GetSeverity() const
{
    return severity_;
} // GetSeverity


CommunicationError ECommException::GetReason() const
{
    return reason_;
} // GetReason


LocString ECommException::GetExtendedInfo() const
{
    return extendedInfo_;
} // GetExtendedInfo


LocString ECommException::GetReasonDescription( CommunicationError reason )
{
    static const std::unordered_map< CommunicationError, LocString > converter{
          { CommunicationError::Timeout,        LOCALIZED( "Timeout expired" ) }
        , { CommunicationError::Checksum,       LOCALIZED( "Invalid checksum" ) }
        , { CommunicationError::NotConnected,   LOCALIZED( "Connection not established" ) }
        , { CommunicationError::SystemError,    LOCALIZED( "System error" ) }
        , { CommunicationError::Unspecified,    LOCALIZED( "Unspecified error" ) }
    };

    auto iter = converter.find( reason );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return LOCALIZED( "?" );
} // GetReasonDescription


LocString ECommException::GetSeverityDescription( ExceptionSeverity severity )
{
    static const std::unordered_map< ExceptionSeverity, LocString > converter{
          { ExceptionSeverity::Error,       LOCALIZED( "Connection reestablish not required" ) }
        , { ExceptionSeverity::Reset,       LOCALIZED( "Connection reestablish required" ) }
        , { ExceptionSeverity::Stop,        LOCALIZED( "Connection reestablish required, but bus need to be stopped" ) }
        , { ExceptionSeverity::WaitRadius,  LOCALIZED( "Bus is temporarily unavailable" ) }
    };

    auto iter = converter.find( severity );
    if ( converter.cend() != iter )
    {
        return iter->second;
    }
    return LOCALIZED( "?" );
} // GetSeverityDescription

} // namespace protocols

} // namespace logika

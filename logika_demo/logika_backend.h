#ifndef LOGIKA_BACKEND_H
#define LOGIKA_BACKEND_H

#include "logika_defs.h"

#include <logika/connections/iconnection.h>
#include <logika/protocols/m4/m4protocol.h>

#include <logika/meters/interval_archive.h>
#include <logika/meters/service_archive.h>

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace Ui { class MainWindow; }

struct LogikaBackend
{
public:
    static LogikaBackend& Instance();
    void InitLogger();
    void LoadResources( const std::string& pathTo, bool silent = true );

    void ConnectTcp( const std::string& address, uint16_t port, uint64_t timeout = 0 );
    void ConnectUdp( const std::string& address, uint16_t port, uint64_t timeout = 0 );
    void ConnectSerial( const std::string& address, uint64_t timeout = 0 );

    void SetProtocolAndDetectMeter();
    void ResetMeterTable();

private:
    LogikaBackend() = default;

public:
    std::shared_ptr< logika::connections::IConnection > connection  = nullptr;
    std::shared_ptr< logika::protocols::M4::M4Protocol > protocol   = nullptr;
    std::shared_ptr< logika::meters::Meter > meter                  = nullptr;

    Ui::MainWindow *ui = nullptr;
};

#endif // LOGIKA_BACKEND_H

#include "mainwindow.h"
#include "logika_backend.h"

#include <QApplication>
#include <locale>

int main(int argc, char *argv[])
{
    std::locale::global( std::locale( "ru_RU.UTF-8" ) );
#if defined( _WIN32 ) || defined( _WIN64 )
    WSADATA wsaData;
    int wsaInitResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if ( 0 != wsaInitResult) {
        return 1;
    }
#endif

    auto& backend = LogikaBackend::Instance(); // Инициализация бэкенда
    backend.InitLogger();
    // Загрузка ресурсов
#if defined( __linux__ ) || defined( __APPLE__ )
    backend.LoadResources( "/home/hyper/bin_data", false );
#endif
#if defined( _WIN32 ) || defined( _WIN64 )
    backend.LoadResources( "E:/projects/res_bin", false ); 
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle( "Logika Libs Demo" );
    w.show();
    return a.exec();
}

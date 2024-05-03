#include "mainwindow.h"
#include "logika_backend.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    auto& backend = LogikaBackend::Instance(); // Инициализация бэкенда
    backend.InitLogger();
    backend.LoadResources( "/home/hyper/bin_data", false ); // Загрузка ресурсов

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle( "Logika Libs Demo" );
    w.show();
    return a.exec();
}

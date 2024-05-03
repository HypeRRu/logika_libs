#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logika_backend.h"
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    LogikaBackend::Instance().ui = ui;
    ui->sidebar->setAlignment( Qt::AlignTop );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_connButton_clicked()
{
    std::string connType = ui->connTypeSelect->currentText().toStdString();
    std::string address = ui->addressEdit->toPlainText().toStdString();
    uint64_t timeout = ui->timeoutEdit->value();
    uint16_t port = 0;
    if ( connType == "TCP" || connType == "UDP" )
    {
        auto sepPos = address.find( ':' );
        if ( address.npos != sepPos )
        {
            try {
                port = std::stoull( address.substr( sepPos + 1 ) );
                address = address.substr(0, sepPos);
            }  catch (...) {}
        }
    }

    LogikaBackend& backend = LogikaBackend::Instance();
    if ( connType == "TCP" )
    {
        backend.ConnectTcp( address, port, timeout );
    }
    else if ( connType == "UDP" )
    {
        backend.ConnectUdp( address, port, timeout );
    }
    else if ( connType == "Serial" )
    {
        backend.ConnectSerial( address, timeout );
    }
    else
    {
        ui->meterParams->setText( "Connection type is invalid" );
        backend.connection = nullptr;
        backend.ResetMeterTable();
        return;
    }

    backend.SetProtocolAndDetectMeter();
}


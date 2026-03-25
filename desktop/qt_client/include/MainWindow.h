#pragma once

#include <QMainWindow>
#include <QListWidget>

#include "HttpServer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onTelemetryReceived(const TelemetryEntry &entry);
    void onServerMessage(const QString &message);

private:
    QListWidget *m_listWidget = nullptr;
    HttpServer m_server;
};
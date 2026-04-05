#pragma once

#include <QMainWindow>
#include <QListWidget>

#include "HttpServer.h"
#include "TelemetryStore.h"

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
    void addTelemetryToUi(const TelemetryEntry &entry);

private:
    QListWidget *m_listWidget = nullptr;
    QListWidget *m_errorListWidget = nullptr;
    HttpServer m_server;
    TelemetryStore m_store;
};

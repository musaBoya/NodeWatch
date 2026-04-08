#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QStringList>

#include "config/AppSettings.h"
#include "server/HttpServer.h"
#include "storage/TelemetryStore.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onTelemetryReceived(const TelemetryEntry &entry);
    void onServerMessage(const QString &message);

private:
    void addTelemetryToUi(const TelemetryEntry &entry);

private:
    QListWidget *m_listWidget = nullptr;
    QListWidget *m_errorListWidget = nullptr;
    QStringList m_startupWarnings;
    AppSettings m_settings;
    HttpServer m_server;
    TelemetryStore m_store;
};

#include "ui/MainWindow.h"

#include "logging/logger.hpp"
#include "rules/TelemetryAlerts.h"

#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
QString joinWarnings(const QStringList &warnings)
{
    return warnings.join(" | ");
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_settings(AppSettings::load(&m_startupWarnings))
{
    setWindowTitle("NodeWatch Desktop");
    resize(800, 500);

    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *layout = new QVBoxLayout(central);

    auto *logList = new QWidget(central);
    auto *errorList = new QWidget(central);

    m_listWidget = new QListWidget(logList);
    m_errorListWidget = new QListWidget(errorList);
    layout->addWidget(m_listWidget,3);
    layout->addWidget(m_errorListWidget,1);


    statusBar()->showMessage(QString("Loaded config: %1").arg(m_settings.configPath), 5000);

    if (!Logger::init(m_settings.logFilePath.toStdString())) {
        const QString loggerError =
            QString("Logger init failed: %1").arg(m_settings.logFilePath);
        statusBar()->showMessage(loggerError, 7000);
        QMessageBox::warning(this, "Logger Error", loggerError);
    } else {
        Logger::set_level(m_settings.logLevel);
        Logger::enable_console_output(true);
        Logger::info(
            QString("NodeWatch Desktop started. Config=%1")
                .arg(m_settings.configPath)
                .toStdString());
    }

    TelemetryAlerts::setHighTemperatureThreshold(m_settings.alertThreshold);
    m_store.setDatabasePath(m_settings.dbPath);

    connect(&m_server, &HttpServer::telemetryReceived,
            this, &MainWindow::onTelemetryReceived);

    connect(&m_server, &HttpServer::serverMessage,
            this, &MainWindow::onServerMessage);

    if (!m_startupWarnings.isEmpty()) {
        const QString warningText = joinWarnings(m_startupWarnings);
        statusBar()->showMessage(warningText, 7000);
        Logger::warning(warningText.toStdString());
    }

    QString dbError;
    if (!m_store.initialize(&dbError)) {
        QMessageBox::warning(this,
                             "Database Error",
                             QString("Failed to initialize SQLite storage.\n%1").arg(dbError));
        statusBar()->showMessage("SQLite initialization failed");
        Logger::error(
            QString("SQLite initialization failed: %1").arg(dbError).toStdString());
    } else {
        QString loadError;
        const QVector<TelemetryEntry> history = m_store.loadRecent(200, &loadError);
        if (!loadError.isEmpty()) {
            statusBar()->showMessage(loadError, 5000);
            Logger::warning(
                QString("Telemetry history load warning: %1").arg(loadError).toStdString());
        } else if (!history.isEmpty()) {
            for (const TelemetryEntry &entry : history) {
                addTelemetryToUi(entry);
            }
            statusBar()->showMessage(
                QString("Loaded %1 telemetry record(s) from SQLite").arg(history.size()),
                5000);
            Logger::info(
                QString("Loaded %1 telemetry record(s) from SQLite")
                    .arg(history.size())
                    .toStdString());
        }
    }

    if (!m_server.start(m_settings.bindAddress, m_settings.port)) {
        statusBar()->showMessage(
            QString("HTTP server failed to start on %1:%2")
                .arg(m_settings.bindAddress)
                .arg(m_settings.port));
        Logger::error(
            QString("HTTP server startup failed on %1:%2")
                .arg(m_settings.bindAddress)
                .arg(m_settings.port)
                .toStdString());
        QMessageBox::critical(this,
                              "Server Error",
                              QString("HTTP server could not start on %1:%2.\n"
                                      "Please ensure the address and port are available.")
                                  .arg(m_settings.bindAddress)
                                  .arg(m_settings.port));
    }
}

MainWindow::~MainWindow()
{
    Logger::info("NodeWatch Desktop shutting down");
    Logger::shutdown();
}

void MainWindow::onTelemetryReceived(const TelemetryEntry &entry)
{
    QString dbError;
    if (!m_store.insertEntry(entry, &dbError)) {
        statusBar()->showMessage(dbError, 5000);
        Logger::error(
            QString("Failed to persist telemetry: %1").arg(dbError).toStdString());
    }

    addTelemetryToUi(entry);
}

void MainWindow::onServerMessage(const QString &message)
{
    statusBar()->showMessage(message, 3000);
}

void MainWindow::addTelemetryToUi(const TelemetryEntry &entry)
{
    auto *item = new QListWidgetItem(entry.toDisplayString());
    if (TelemetryAlerts::isHighTemperature(entry)) {
        auto *item2 = new QListWidgetItem(entry.toDisplayString());
        item->setForeground(Qt::red);
        item2->setForeground(Qt::red);
        m_errorListWidget->addItem(item2);
        m_errorListWidget->scrollToBottom();
    }
    m_listWidget->addItem(item);
    m_listWidget->scrollToBottom();
}

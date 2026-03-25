#include "MainWindow.h"

#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("NodeWatch Desktop");
    resize(800, 500);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    m_listWidget = new QListWidget(central);
    layout->addWidget(m_listWidget);

    setCentralWidget(central);
    statusBar()->showMessage("Ready");

    connect(&m_server, &HttpServer::telemetryReceived,
            this, &MainWindow::onTelemetryReceived);

    connect(&m_server, &HttpServer::serverMessage,
            this, &MainWindow::onServerMessage);

    m_server.start(8080);
}

void MainWindow::onTelemetryReceived(const TelemetryEntry &entry)
{
    m_listWidget->addItem(entry.toDisplayString());
    m_listWidget->scrollToBottom();
}

void MainWindow::onServerMessage(const QString &message)
{
    statusBar()->showMessage(message, 3000);
}
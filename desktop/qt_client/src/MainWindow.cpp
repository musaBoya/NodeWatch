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
    setCentralWidget(central);
    auto *layout = new QVBoxLayout(central);

    auto *logList = new QWidget(central);
    auto *errorList = new QWidget(central);

    m_listWidget = new QListWidget(logList);
    m_errorListWidget = new QListWidget(errorList);
    layout->addWidget(m_listWidget,3);
    layout->addWidget(m_errorListWidget,1);


    statusBar()->showMessage("Ready");

    connect(&m_server, &HttpServer::telemetryReceived,
            this, &MainWindow::onTelemetryReceived);

    connect(&m_server, &HttpServer::serverMessage,
            this, &MainWindow::onServerMessage);

    m_server.start(8080);
}

void MainWindow::onTelemetryReceived(const TelemetryEntry &entry)
{
    auto *item = new QListWidgetItem(entry.toDisplayString());
    if (entry.temperature > 26) {
        auto *item2 = new QListWidgetItem(entry.toDisplayString());
        item->setForeground(Qt::red);
        item2->setForeground(Qt::red);
        m_errorListWidget->addItem(item2);
        m_errorListWidget->scrollToBottom();
    }
    else {
        item->setForeground(Qt::white);
    }
    m_listWidget->addItem(item);
    m_listWidget->scrollToBottom();
}

void MainWindow::onServerMessage(const QString &message)
{
    statusBar()->showMessage(message, 3000);
}
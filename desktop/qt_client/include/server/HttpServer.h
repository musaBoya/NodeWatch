#pragma once

#include <QObject>
#include <QString>
#include <QTcpServer>

#include "models/TelemetryEntry.h"

class QHttpServer;

class HttpServer : public QObject
{
    Q_OBJECT

public:
    explicit HttpServer(QObject *parent = nullptr);
    bool start(const QString &bindAddress, quint16 port);

signals:
    void telemetryReceived(const TelemetryEntry &entry);
    void serverMessage(const QString &message);

private:
    bool parseTelemetry(const QByteArray &body, TelemetryEntry &entry) const;

private:
    QHttpServer *m_server = nullptr;
};

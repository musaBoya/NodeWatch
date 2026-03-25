#include "HttpServer.h"

#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>
#include <QVariant>

HttpServer::HttpServer(QObject *parent)
    : QObject(parent),
      m_server(new QHttpServer(this))
{
    m_server->route(
        "/telemetry",
        QHttpServerRequest::Method::Post,
        [this](const QHttpServerRequest &request)
        {
            TelemetryEntry entry;

            if (!parseTelemetry(request.body(), entry)) {
                emit serverMessage("Invalid telemetry payload recieved");
                return QHttpServerResponse(
                    QByteArray("Invalid payload"),
                    QHttpServerResponse::StatusCode::BadRequest);
            }

            emit telemetryReceived(entry);
            emit serverMessage(QString("Telemetry recieved: %1").arg(entry.deviceId));

            return QHttpServerResponse(
                QByteArray("OK"),
                QHttpServerResponse::StatusCode::Ok);
        });
}

bool HttpServer::start(quint16 port)
{
    auto *tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, port)) {
        emit serverMessage("TCP server is not started");
        return false;
    }

    m_server->bind(tcpServer);

    emit serverMessage(QString("The HTTP server is listening on port %1.").arg(port));
    return true;
}

bool HttpServer::parseTelemetry(const QByteArray &body, TelemetryEntry &entry) const
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    const QJsonObject obj = doc.object();

    entry.deviceId = obj.value("device_id").toString();
    entry.temperature = obj.value("temperature").toDouble();
    entry.humidity = obj.value("humidity").toDouble();
    entry.status = obj.value("status").toString();

    const QVariant tsVariant = obj.value("timestamp").toVariant();
    bool ok = false;
    entry.timestamp = tsVariant.toLongLong(&ok);

    if (!ok) {
        return false;
    }

    if (entry.deviceId.isEmpty()) {
        return false;
    }

    return true;
}
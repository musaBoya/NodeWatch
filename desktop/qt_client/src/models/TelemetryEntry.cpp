#include "models/TelemetryEntry.h"

#include <QDateTime>

QString TelemetryEntry::toDisplayString() const
{
    const QString timeStr =
        QDateTime::fromSecsSinceEpoch(timestamp).toString("HH:mm:ss");

    return QString("[%1] %2 | temp=%3 | hum=%4 | status=%5")
        .arg(timeStr)
        .arg(deviceId)
        .arg(temperature, 0, 'f', 1)
        .arg(humidity, 0, 'f', 1)
        .arg(status);
}

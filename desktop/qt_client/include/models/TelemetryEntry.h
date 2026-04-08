#pragma once

#include <QString>
#include <QMetaType>

struct TelemetryEntry
{
    QString deviceId;
    qint64 timestamp = 0;
    double temperature = 0.0;
    double humidity = 0.0;
    QString status;

    QString toDisplayString() const;
};

Q_DECLARE_METATYPE(TelemetryEntry)
#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QVector>

#include "TelemetryEntry.h"

class TelemetryStore
{
public:
    TelemetryStore();
    ~TelemetryStore();

    bool initialize(QString *errorMessage = nullptr);
    bool insertEntry(const TelemetryEntry &entry, QString *errorMessage = nullptr);
    QVector<TelemetryEntry> loadRecent(int limit, QString *errorMessage = nullptr);

private:
    bool openDatabase(QString *errorMessage);
    bool ensureSchema(QString *errorMessage);

private:
    QString m_connectionName;
    QSqlDatabase m_db;
};

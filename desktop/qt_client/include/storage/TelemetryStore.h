#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QVector>

#include "models/TelemetryEntry.h"

class TelemetryStore
{
public:
    TelemetryStore();
    ~TelemetryStore();

    void setDatabasePath(const QString &path);
    bool initialize(QString *errorMessage = nullptr);
    bool insertEntry(const TelemetryEntry &entry, QString *errorMessage = nullptr);
    QVector<TelemetryEntry> loadRecent(int limit, QString *errorMessage = nullptr);

private:
    bool openDatabase(QString *errorMessage);
    bool ensureSchema(QString *errorMessage);

private:
    QString m_connectionName;
    QString m_databasePath;
    QSqlDatabase m_db;
};

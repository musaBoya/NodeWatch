#include "TelemetryStore.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>
#include <QtGlobal>

TelemetryStore::TelemetryStore()
    : m_connectionName(QString("nodewatch_conn_%1").arg(reinterpret_cast<quintptr>(this), 0, 16))
{
}

TelemetryStore::~TelemetryStore()
{
    if (m_db.isValid() && m_db.isOpen()) {
        m_db.close();
    }

    if (!m_connectionName.isEmpty()) {
        const QString connectionName = m_connectionName;
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
}

bool TelemetryStore::initialize(QString *errorMessage)
{
    if (!openDatabase(errorMessage)) {
        return false;
    }

    return ensureSchema(errorMessage);
}

bool TelemetryStore::openDatabase(QString *errorMessage)
{
    if (m_db.isValid() && m_db.isOpen()) {
        return true;
    }

    const QString appDataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appDataDir.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Failed to resolve app data directory";
        }
        return false;
    }

    QDir dir(appDataDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        if (errorMessage != nullptr) {
            *errorMessage = QString("Failed to create app data directory: %1").arg(appDataDir);
        }
        return false;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_db.setDatabaseName(dir.filePath("nodewatch.db"));

    if (!m_db.open()) {
        if (errorMessage != nullptr) {
            *errorMessage = QString("Failed to open SQLite database: %1")
                                .arg(m_db.lastError().text());
        }
        return false;
    }

    return true;
}

bool TelemetryStore::ensureSchema(QString *errorMessage)
{
    QSqlQuery query(m_db);
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS telemetry ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "device_id TEXT NOT NULL,"
            "timestamp INTEGER NOT NULL,"
            "temperature REAL NOT NULL,"
            "humidity REAL NOT NULL,"
            "status TEXT NOT NULL"
            ")")) {
        if (errorMessage != nullptr) {
            *errorMessage = QString("Failed to create telemetry table: %1")
                                .arg(query.lastError().text());
        }
        return false;
    }

    return true;
}

bool TelemetryStore::insertEntry(const TelemetryEntry &entry, QString *errorMessage)
{
    if (!m_db.isOpen()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Database is not open";
        }
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO telemetry (device_id, timestamp, temperature, humidity, status) "
        "VALUES (:device_id, :timestamp, :temperature, :humidity, :status)");

    query.bindValue(":device_id", entry.deviceId);
    query.bindValue(":timestamp", entry.timestamp);
    query.bindValue(":temperature", entry.temperature);
    query.bindValue(":humidity", entry.humidity);
    query.bindValue(":status", entry.status);

    if (!query.exec()) {
        if (errorMessage != nullptr) {
            *errorMessage = QString("Failed to insert telemetry row: %1")
                                .arg(query.lastError().text());
        }
        return false;
    }

    return true;
}

QVector<TelemetryEntry> TelemetryStore::loadRecent(int limit, QString *errorMessage)
{
    QVector<TelemetryEntry> entries;

    if (!m_db.isOpen()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Database is not open";
        }
        return entries;
    }

    if (limit <= 0) {
        return entries;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT device_id, timestamp, temperature, humidity, status "
        "FROM telemetry "
        "ORDER BY id DESC "
        "LIMIT :limit");
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        if (errorMessage != nullptr) {
            *errorMessage = QString("Failed to load telemetry history: %1")
                                .arg(query.lastError().text());
        }
        return entries;
    }

    while (query.next()) {
        TelemetryEntry entry;
        entry.deviceId = query.value(0).toString();
        entry.timestamp = query.value(1).toLongLong();
        entry.temperature = query.value(2).toDouble();
        entry.humidity = query.value(3).toDouble();
        entry.status = query.value(4).toString();
        entries.prepend(entry);
    }

    return entries;
}

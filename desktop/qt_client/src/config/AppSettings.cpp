#include "config/AppSettings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QHostAddress>
#include <QSettings>
#include <QVariant>
#include <QtGlobal>

namespace
{
constexpr quint16 kDefaultPort = 8080;
constexpr double kDefaultAlertThreshold = 26.0;

const QString kDefaultBindAddress = QStringLiteral("0.0.0.0");
const QString kDefaultDbPath = QStringLiteral("nodewatch.db");
const QString kDefaultLogFilePath = QStringLiteral("nodewatch.log");
const QString kDefaultLogLevel = QStringLiteral("info");

void addWarning(QStringList *warnings, const QString &message)
{
    if (warnings != nullptr) {
        warnings->append(message);
    }
}

LogLevel parseLogLevel(const QString &rawValue, bool *ok)
{
    const QString normalized = rawValue.trimmed().toLower();

    if (normalized == "debug") {
        if (ok != nullptr) {
            *ok = true;
        }
        return LogLevel::Debug;
    }

    if (normalized == "info") {
        if (ok != nullptr) {
            *ok = true;
        }
        return LogLevel::Info;
    }

    if (normalized == "warning") {
        if (ok != nullptr) {
            *ok = true;
        }
        return LogLevel::Warning;
    }

    if (normalized == "error") {
        if (ok != nullptr) {
            *ok = true;
        }
        return LogLevel::Error;
    }

    if (ok != nullptr) {
        *ok = false;
    }
    return LogLevel::Info;
}

QString resolvePathFromConfig(const QString &configPath, const QString &configuredPath)
{
    const QFileInfo pathInfo(configuredPath);
    if (pathInfo.isAbsolute()) {
        return pathInfo.filePath();
    }

    const QFileInfo configFileInfo(configPath);
    const QDir configDir(configFileInfo.absolutePath());
    return configDir.filePath(configuredPath);
}
}

QString appLogLevelToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:
        return "debug";
    case LogLevel::Info:
        return "info";
    case LogLevel::Warning:
        return "warning";
    case LogLevel::Error:
        return "error";
    }

    return "info";
}

QString AppSettings::resolveConfigPath()
{
    const QString envPath = qEnvironmentVariable("NODEWATCH_CONFIG").trimmed();
    if (!envPath.isEmpty()) {
        return QFileInfo(envPath).absoluteFilePath();
    }

    return QDir(QCoreApplication::applicationDirPath()).filePath("nodewatch.ini");
}

AppSettings AppSettings::load(QStringList *warnings)
{
    AppSettings loaded;
    loaded.configPath = resolveConfigPath();

    const QFileInfo configFileInfo(loaded.configPath);
    QDir configDir(configFileInfo.absolutePath());
    if (!configDir.exists() && !configDir.mkpath(".")) {
        addWarning(warnings,
                   QString("Failed to create config directory: %1")
                       .arg(configDir.absolutePath()));
    }

    QSettings settings(loaded.configPath, QSettings::IniFormat);
    settings.setFallbacksEnabled(false);

    const auto ensureDefault = [&settings](const QString &key, const QVariant &value)
    {
        if (!settings.contains(key)) {
            settings.setValue(key, value);
        }
    };

    ensureDefault("server/port", kDefaultPort);
    ensureDefault("server/bind_address", kDefaultBindAddress);
    ensureDefault("storage/db_path", kDefaultDbPath);
    ensureDefault("alerts/temperature_threshold", kDefaultAlertThreshold);
    ensureDefault("logging/level", kDefaultLogLevel);
    ensureDefault("logging/file_path", kDefaultLogFilePath);

    bool portOk = false;
    int configuredPort = settings.value("server/port", kDefaultPort).toInt(&portOk);
    if (!portOk || configuredPort <= 0 || configuredPort > 65535) {
        addWarning(warnings,
                   QString("Invalid server port. Falling back to %1").arg(kDefaultPort));
        configuredPort = kDefaultPort;
        settings.setValue("server/port", configuredPort);
    }
    loaded.port = static_cast<quint16>(configuredPort);

    QString configuredBindAddress =
        settings.value("server/bind_address", kDefaultBindAddress).toString().trimmed();
    if (configuredBindAddress.compare("any", Qt::CaseInsensitive) == 0 ||
        configuredBindAddress == "*") {
        configuredBindAddress = kDefaultBindAddress;
        settings.setValue("server/bind_address", configuredBindAddress);
    }

    QHostAddress bindAddressCandidate;
    if (configuredBindAddress.isEmpty() ||
        !bindAddressCandidate.setAddress(configuredBindAddress)) {
        addWarning(warnings,
                   QString("Invalid bind address. Falling back to %1")
                       .arg(kDefaultBindAddress));
        configuredBindAddress = kDefaultBindAddress;
        settings.setValue("server/bind_address", configuredBindAddress);
    }
    loaded.bindAddress = configuredBindAddress;

    QString configuredDbPath =
        settings.value("storage/db_path", kDefaultDbPath).toString().trimmed();
    if (configuredDbPath.isEmpty()) {
        configuredDbPath = kDefaultDbPath;
        settings.setValue("storage/db_path", configuredDbPath);
        addWarning(warnings,
                   QString("Empty db path. Falling back to %1").arg(kDefaultDbPath));
    }
    loaded.dbPath = resolvePathFromConfig(loaded.configPath, configuredDbPath);

    bool thresholdOk = false;
    double configuredThreshold =
        settings.value("alerts/temperature_threshold", kDefaultAlertThreshold)
            .toDouble(&thresholdOk);
    if (!thresholdOk || !qIsFinite(configuredThreshold)) {
        addWarning(warnings,
                   QString("Invalid alert threshold. Falling back to %1")
                       .arg(kDefaultAlertThreshold));
        configuredThreshold = kDefaultAlertThreshold;
        settings.setValue("alerts/temperature_threshold", configuredThreshold);
    }
    loaded.alertThreshold = configuredThreshold;

    const QString configuredLogLevel =
        settings.value("logging/level", kDefaultLogLevel).toString();
    bool logLevelOk = false;
    loaded.logLevel = parseLogLevel(configuredLogLevel, &logLevelOk);
    if (!logLevelOk) {
        addWarning(warnings, "Invalid log level. Falling back to info");
        loaded.logLevel = LogLevel::Info;
    }
    settings.setValue("logging/level", appLogLevelToString(loaded.logLevel));

    QString configuredLogFilePath =
        settings.value("logging/file_path", kDefaultLogFilePath).toString().trimmed();
    if (configuredLogFilePath.isEmpty()) {
        configuredLogFilePath = kDefaultLogFilePath;
        settings.setValue("logging/file_path", configuredLogFilePath);
        addWarning(warnings,
                   QString("Empty log file path. Falling back to %1")
                       .arg(kDefaultLogFilePath));
    }
    loaded.logFilePath = resolvePathFromConfig(loaded.configPath, configuredLogFilePath);

    settings.sync();
    if (settings.status() != QSettings::NoError) {
        addWarning(warnings,
                   QString("Failed to fully persist config at %1").arg(loaded.configPath));
    }

    return loaded;
}

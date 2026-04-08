#pragma once

#include <QString>
#include <QStringList>
#include <QtGlobal>

#include "logging/log_level.hpp"

struct AppSettings
{
    quint16 port = 8080;
    QString bindAddress = "0.0.0.0";
    QString dbPath;
    double alertThreshold = 26.0;
    LogLevel logLevel = LogLevel::Info;
    QString logFilePath;
    QString configPath;

    static AppSettings load(QStringList *warnings = nullptr);
    static QString resolveConfigPath();
};

QString appLogLevelToString(LogLevel level);

#include "rules/TelemetryAlerts.h"

#include <QtGlobal>

namespace
{
double g_temperatureAlertThreshold = 26.0;
}

void TelemetryAlerts::setHighTemperatureThreshold(double threshold)
{
    if (qIsFinite(threshold)) {
        g_temperatureAlertThreshold = threshold;
    }
}

double TelemetryAlerts::highTemperatureThreshold()
{
    return g_temperatureAlertThreshold;
}

bool TelemetryAlerts::isHighTemperature(const TelemetryEntry &entry)
{
    return entry.temperature > g_temperatureAlertThreshold;
}

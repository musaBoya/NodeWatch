#pragma once

#include "models/TelemetryEntry.h"

namespace TelemetryAlerts
{
void setHighTemperatureThreshold(double threshold);
double highTemperatureThreshold();
bool isHighTemperature(const TelemetryEntry &entry);
}

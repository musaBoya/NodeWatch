#include <QApplication>
#include <QMetaType>

#include "models/TelemetryEntry.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<TelemetryEntry>("TelemetryEntry");

    MainWindow window;
    window.show();

    return app.exec();
}

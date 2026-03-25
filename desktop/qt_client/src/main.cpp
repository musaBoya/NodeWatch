#include <QApplication>
#include <QMetaType>

#include "MainWindow.h"
#include "TelemetryEntry.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<TelemetryEntry>("TelemetryEntry");

    MainWindow window;
    window.show();

    return app.exec();
}
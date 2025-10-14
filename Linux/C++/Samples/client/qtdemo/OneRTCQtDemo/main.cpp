#include "MainWindow.h"

#include <QApplication>
#include "RtcEngineObject.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RtcEngineObject::registerRtcEngineTypes();

    MainWindow w;
    w.show();

    if (QGuiApplication::platformName() == QLatin1String("xcb")) {
        qDebug("running on x11");
    } else {
        qDebug("running on wayland");
    }

    return a.exec();
}

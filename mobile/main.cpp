#include <QtWidgets/QApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>
#include <QtCore/QDir>
#include <QQmlApplicationEngine>

#include "remoteconnection.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Qt Charts uses Qt Graphics View Framework for drawing, therefore QApplication must be used.
    QApplication app(argc, argv);
    RemoteConnection remoteConnection;
    qmlRegisterType<RemoteConnection>("Mobile", 1, 0, "RemoteConnection");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("remoteConnection", &remoteConnection);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

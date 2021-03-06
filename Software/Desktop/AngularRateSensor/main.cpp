#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    app.setOrganizationName("Some Company");
    app.setOrganizationDomain("somecompany.com");
    app.setApplicationName("Amazing Application");

    qmlRegisterType<Controller>("custom.controller", 1, 0, "Controller");
   // qmlRegisterType<MainModel>("custom.controller", 1, 0, "MainModel");

    qRegisterMetaType<MainModel>("MainModel");



    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:///main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "calculator.h"
#include "plotdeclarativeitem.h"
#include "mainwindow.h"

//deactivate for QWidget Gui
#define QML_GUI

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
   #ifdef QML_GUI
    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

    //Register the calculator with the QML engine
    qmlRegisterType<Calculator>("cppComponents", 1, 0, "Calculator");
    qmlRegisterType<PlotDeclarativeItem>("cppComponents", 1, 0, "PlotDeclarativeItem");

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer->setMainQmlFile(QLatin1String("qml/QmlCalculator/main.qml"));
    viewer->showExpanded();

   #else //use widgets
    MainWindow mainWindow;
    mainWindow.show();
   #endif
    return app->exec();
}

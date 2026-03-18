#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"
#include "theme.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    app.setStyle(QStyleFactory::create("Fusion"));
    app.setApplicationName("Sprite-Tools");
    app.setPalette(Theme::darkPalette());
    app.setStyleSheet(Theme::globalStyleSheet());

    MainWindow w;
    w.resize(1280, 720);
    w.show();

    if (argc > 1)
        w.openFile(QString::fromLocal8Bit(argv[1]));

    return app.exec();
}
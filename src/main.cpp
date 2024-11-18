#include <QApplication>
#include <QDebug>
#include <vector>
#include <QObject>
#include <QFile>

#include "ui/mainwindow.h"
#include "core/config.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QString path = Config::getInstance().findStyleSheet("main");
    // read style sheet 
    if (!path.isEmpty()) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            app.setStyleSheet(file.readAll());
        }
    };

    MainWindow window;
    window.show();
    return app.exec();
}
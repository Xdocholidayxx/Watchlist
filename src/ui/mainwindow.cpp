#include <QMainWindow>
#include <QWidget>

#include "ui/mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
}

MainWindow::~MainWindow() {
}
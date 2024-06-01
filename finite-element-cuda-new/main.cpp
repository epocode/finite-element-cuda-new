#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(":/MainWindows/ManjaroMix.qss");
    file.open(QFile::ReadOnly);
    QString qss = file.readAll();
    a.setStyleSheet(qss);
    file.close();
    MainWindow w;
    w.show();
    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //PlayWithSelf p;
    w.show();

    //QObject::connect(w.play, SIGNAL(backClicked()), &w, SLOT(Back()));

    return a.exec();
}

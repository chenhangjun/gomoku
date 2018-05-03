#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "playwithself.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void ShowLabel();

public Q_SLOTS:
    void Button1Clicked(); //按钮点击事件

private:
    Ui::MainWindow *ui;
    PlayWithSelf *play;   //按钮跳转对象
};

#endif // MAINWINDOW_H

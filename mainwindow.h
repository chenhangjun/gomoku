#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

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

private Q_SLOTS:
    void Button1Clicked(); //按钮1点击事件
    void Button2Clicked();  //按钮2点击事件

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H

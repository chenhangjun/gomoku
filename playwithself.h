#ifndef PLAYWITHSELF_H
#define PLAYWITHSELF_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QtGui>

namespace Ui {
class PlayWithSelf;
}

class PlayWithSelf : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayWithSelf(QWidget *parent = 0);
    ~PlayWithSelf();
    void paintEvent(QPaintEvent *);   //重写父类函数，名称不可该
    void mouseReleaseEvent(QMouseEvent *); //重写父类函数，名称不可改
    void mouseMoveEvent(QMouseEvent *);  //改变鼠标状态

    void PlayRandom(); //掷骰子
    void ShowStatus();  //显示游戏时信息
    void CountDown();  //显示倒计时



public Q_SLOTS:
    void ShowRandom();  //掷骰子事件
    void StartPlay(); //销毁控件

private:
    Ui::PlayWithSelf *ui;
    int chessboard[16][16];   //横纵16个着棋点
    bool JudgeWin(int, int);  //是否赢了
    int player;    // -1代表白方，1代表黑方；
    int flag;  //是否掷过骰子
    int countdown = 30;  //倒计时30s
    QPushButton *button;  //掷骰子按钮
    QPushButton *button1; //掷骰子结束后OK按钮
    QLabel *label;   //显示骰子的标签
    QLabel *label1;  //显示谁先手的标签
    QLabel *info1;  //显示先手方
    QLabel *info2 = new QLabel(this);  //显示当前行棋方
};

#endif // PLAYWITHSELF_H
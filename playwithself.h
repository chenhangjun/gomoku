#ifndef PLAYWITHSELF_H
#define PLAYWITHSELF_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QtGui>
#include <QLCDNumber>

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
    void Timer();  //倒计时



private Q_SLOTS:
    void ShowRandom();  //掷骰子事件
    void StartPlay(); //销毁控件
    void CountDown();  //倒计时
    void Undo();  //悔棋
    void Exit();  // 点击OK返回
    void Again();  //再玩一局
    void onBackClicked();  //与点击信号发射有关槽函数

private:
    Ui::PlayWithSelf *ui;
    int chessboard[16][16];   //横纵16个着棋点
    bool JudgeWin(int, int);  //是否赢了
    int player;    // -1代表白方，1代表黑方；
    int flag;  //是否掷过骰子
    int full;  //判断是否棋盘填满而平局
    int countdown;  //倒计时30s
    int timecounter;  //计时间， 1000ms
    int x, y; //鼠标对应位置的二维坐标
    int wx, wy, bx, by;  //黑白双方最后落子坐标
    int firx, firy, lasx, lasy;  //五子连珠始末坐标
    int undowx,undowy, undobx, undoby;  //悔棋前备份

    QPushButton *button;  //掷骰子按钮
    QPushButton *button1; //掷骰子结束后OK按钮
    QPushButton *undo;    //悔棋按钮
    QPushButton *clear;   //再来一局按钮
    QPushButton *back;    //返回主界面按钮
    QLabel *label;   //显示骰子的标签
    QLabel *label1;  //显示谁先手的标签
    QLabel *info1;  //显示先手方
    QLabel *info2;  //显示当前行棋方
    QLabel *info3;  //"行棋倒计时"label
    QLCDNumber *lcdNumber;  //显示倒计时
    QTimer *timer;  //计时器
    QDialog *subWin;  // 对局结束子窗口

signals:
    void backClicked();  //点击信号

};

#endif // PLAYWITHSELF_H

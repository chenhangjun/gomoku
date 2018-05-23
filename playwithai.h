#ifndef PLAYWITHAI_H
#define PLAYWITHAI_H

#include <QMainWindow>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QtGui>
#include <QDialog>
#include <QLCDNumber>
#include "stack.h"
#include "point.h"

namespace Ui {
class PlayWithAI;
}

class PlayWithAI : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayWithAI(QWidget *parent = 0);
    ~PlayWithAI();
    void paintEvent(QPaintEvent *);   //重写父类函数，名称不可该
    void mouseReleaseEvent(QMouseEvent *); //重写父类函数，名称不可改
    void mouseMoveEvent(QMouseEvent *);  //改变鼠标状态
    void ChooseColor();

private Q_SLOTS:
    void Start();
    void Choose();
    void Choose1();
    void CountDown();
    void Undo();
    void Exit();  // 点击OK返回
    void Again();  //再玩一局
    void onBackClicked();  //与点击信号发射有关槽函数

private:
    Ui::PlayWithAI *ui;
    int countdown;
    int timecounter;  //计时间， 1000ms
    int chessboard[16][16];   //横纵16个着棋点
    int scoreboard[16][16][5];   //每个点的价值大小
    int valuelist[2][6];    //价值策略
    int player;
    int x, y;  //鼠标坐标
    int flag;  // flag == 0时还不能下棋
    int full;  //棋盘满
    int colorchosen;  //是否已选择棋色
    int orderchosen;  //是否已选择先后手
    int wx, wy, bx, by;  //黑白双方最后落子坐标
    int px, py;
    int firx, firy, lasx, lasy;  //五子连珠始末坐标
    int totalcnt; //计数总棋子数
    int listcnt;  //计数栈

    Point *list;  //存入栈
    Stack<int> stk;  //栈，悔棋用

    QRadioButton *white;  //选择白
    QRadioButton *black;  //选择黑
    QRadioButton *first;  //选择先手
    QRadioButton *last;   //选择后手
    QButtonGroup *color;  //QRadioButton颜色选择按钮组
    QButtonGroup *order;  //QRadioButton次序选择按钮组
    QPushButton *start;   //开始游戏
    QPushButton *undo;    //撤销按钮
    QPushButton *clear;   //再来一局按钮
    QPushButton *back;    //返回主界面按钮
    QLabel *label;        //"行棋倒计时"标签
    QLCDNumber *lcdNumber;  //显示倒计时
    QTimer *timer;  //计时器
    QDialog *subWin;      //游戏结束时显示信息

    void initValue();   //初始化valuelist数组
    void Score(int, int);
    bool isLegal(int, int);
    void Timer();


    //4个搜索方向
    void dir1(int, int);
    void dir2(int, int);
    void dir3(int, int);
    void dir4(int, int);

    void AIPlay();   //电脑落子
    bool JudgeWin(int, int);


};

#endif // PLAYWITHAI_H

#ifndef AIUPDATE_H
#define AIUPDATE_H

#include <QMainWindow>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QtGui>
#include <QDialog>

namespace Ui {
class AiUpdate;
}

class AiUpdate : public QMainWindow
{
    Q_OBJECT

public:
    explicit AiUpdate(QWidget *parent = 0);
    ~AiUpdate();
    void paintEvent(QPaintEvent *);   //重写父类函数，名称不可该
    void mouseReleaseEvent(QMouseEvent *); //重写父类函数，名称不可改
    void ChooseColor();

private Q_SLOTS:
    void Start();
    void Choose();

private:
    Ui::AiUpdate *ui;
    int chessboard[16][16];   //横纵16个着棋点
    int scoreboard[16][16][5];   //每个点的价值大小
    int valuelist[2][6];    //价值策略
    int player;
    int x, y;  //鼠标坐标
    int flag;  // flag == 0时还不能下棋

    QRadioButton *white;  //选择白
    QRadioButton *black;  //选择黑
    QButtonGroup *color;  //QRadioButton颜色选择按钮组
    QPushButton *start;   //开始游戏
    QDialog *subWin;      //游戏结束时显示信息

    void initValue();   //初始化valuelist数组
    void Score(int, int);
    bool isLegal(int, int);
    //4个搜索方向
    void dir1(int, int);
    void dir2(int, int);
    void dir3(int, int);
    void dir4(int, int);

    void AIPlay();   //电脑落子
    bool JudgeWin(int, int);

};

#endif // AIUPDATE_H

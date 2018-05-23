#include "aiupdate.h"
#include "ui_aiupdate.h"
#include <cmath>
#include <QPainter>
#include <QSound>
#include "mainwindow.h"

AiUpdate::AiUpdate(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AiUpdate)
{
    ui->setupUi(this);
    setFixedSize(970, 640);

    full = 1;
    wx = -20;    wy = -20;
    bx = -20;    by = -20;
    firx = 0;    firy = 0;
    lasx = 0;    lasy = 0;
    totalcnt = 0;
    listcnt = 1;

    list = new Point[250];

    memset(chessboard, 0, sizeof(chessboard));
    memset(scoreboard, 0, sizeof(scoreboard));
    player = 1;
    full = 1;
    flag = 0;
    initValue();
    colorchosen = 0;
    orderchosen = 0;

    ChooseColor();

}

AiUpdate::~AiUpdate()
{
    //delete ui;
}

//初始化valuelist
void AiUpdate::initValue()
{
    memset(valuelist, 0, sizeof(valuelist));
    valuelist[0][1] = 10;
    for(int i = 2; i < 6; i++) {
        valuelist[0][i] = valuelist[0][i - 1] * 10;
        valuelist[1][i - 1] = valuelist[0][i - 2];
    }
    valuelist[1][5] = 100000;

    scoreboard[7][7][0] = 1;
    /*初始化结果如下  第0列无用
    valuelist = { { 0, 10, 100, 1000, 10000, 100000 },    //活子
                  { 0, 0,  10,  100,  1000,  100000} };   //死子

    */
}

void AiUpdate::paintEvent(QPaintEvent *)  //绘制棋盘
{
    QPainter p(this);

    p.drawPixmap(0, 0, 970, 640, QPixmap(":/background.jpg"));

    p.setRenderHint(QPainter::Antialiasing, true); //消除锯齿

    for(int i = 1; i < 16; i++) {
        p.drawLine(40, i * 40, 600, i * 40); //画横线,始末点确定线段
        p.drawLine(i * 40, 40, i * 40, 600); //画纵线，始末点确定线段
    }

    QPen pen;
    pen.setWidth(3);
    p.setPen(pen);
    p.drawLine(30, 30, 30, 610);
    p.drawLine(30, 30, 610, 30);
    p.drawLine(610, 30, 610, 610);
    p.drawLine(30, 610, 610, 610);

    //线条粗细复原
    pen.setWidth(1);
    p.setPen(pen);

    QBrush brush;      //创建画刷
    brush.setStyle(Qt::SolidPattern); //填充风格

    //画天元和小星
    brush.setColor(Qt::black);  // 设置画刷颜色为黑
    p.setBrush(brush);  //使用画刷
    //QPoint(x, y)确定圆心，(4, 4)为椭圆横轴和纵轴(圆半径)
    p.drawEllipse(QPoint(8 * 40, 8 * 40), 4, 4);    //天元
    p.drawEllipse(QPoint(4 * 40, 4 * 40), 4, 4);    //左上星
    p.drawEllipse(QPoint(4 * 40, 12 * 40), 4, 4);   //左下星
    p.drawEllipse(QPoint(12 * 40, 4 * 40), 4, 4);   //右上星
    p.drawEllipse(QPoint(12 * 40, 12 * 40), 4, 4);  // 右下星

    //最后落子定位
    p.setPen(QPen(Qt::red));

    int p1, p2;
    if(!stk.isEmpty()) {
        p1 = stk.peek();
        wx = list[p1].getPX();
        wy = list[p1].getPY();
    } else {
        wx = -20;    wy = -20;
    }
    if(stk.Size() > 1) {
        p2 = stk.peek2();
        bx = list[p2].getPX();
        by = list[p2].getPY();
    } else{
        bx = -20;    by = -20;
    }

    p.drawLine(wx - 18, wy - 18, wx - 18, wy - 8);
    p.drawLine(wx - 18, wy - 18, wx - 8, wy - 18);
    p.drawLine(wx - 18, wy + 18, wx - 18, wy + 8);
    p.drawLine(wx - 18, wy + 18, wx - 8, wy + 18);
    p.drawLine(wx + 18, wy - 18, wx + 8, wy - 18);
    p.drawLine(wx + 18, wy - 18, wx + 18, wy - 8);
    p.drawLine(wx + 18, wy + 18, wx + 18, wy + 8);
    p.drawLine(wx + 18, wy + 18, wx + 8, wy + 18);

    p.drawLine(bx - 18, by - 18, bx - 18, by - 8);
    p.drawLine(bx - 18, by - 18, bx - 8, by - 18);
    p.drawLine(bx - 18, by + 18, bx - 18, by + 8);
    p.drawLine(bx - 18, by + 18, bx - 8, by + 18);
    p.drawLine(bx + 18, by - 18, bx + 8, by - 18);
    p.drawLine(bx + 18, by - 18, bx + 18, by - 8);
    p.drawLine(bx + 18, by + 18, bx + 18, by + 8);
    p.drawLine(bx + 18, by + 18, bx + 8, by + 18);

    p.setPen(QPen(Qt::black));

    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            if(chessboard[i][j] == 1) {  //该点上是黑子
                brush.setColor(Qt::black);  // 设置画刷颜色为黑
                p.setBrush(brush);  //使用画刷
                //QPoint(x, y)确定圆心，(15, 15)为椭圆横轴和纵轴(圆半径)
                p.drawEllipse(QPoint((i + 1) * 40, (j + 1) * 40), 15, 15);
            } else if(chessboard[i][j] == -1) {  //该点上是白子
                brush.setColor(Qt::white);  //设置画刷颜色为白
                p.setBrush(brush);  //使用画刷
                p.drawEllipse(QPoint((i + 1) * 40, (j + 1) * 40), 15, 15);
            }
        }
    }

    //五子连线画线
    if((abs(firx - lasx) >= 160) || (abs(firy - lasy) >= 160)) {
        pen.setWidth(3);
        pen.setColor(Qt::red);
        p.setPen(pen);
        p.drawLine(firx, firy, lasx, lasy);
    }

}

void AiUpdate::mouseReleaseEvent(QMouseEvent *e)
{
    if(flag == 0) {    //未选棋色或先后手
        return;
    }

    ui->centralwidget->setMouseTracking(true);
    this->setMouseTracking(true);
    mouseMoveEvent(e);

    x = (e->x() - 25) / 40;  //棋点横坐标
    y = (e->y() - 25) / 40;  //棋点纵坐标


    if(isLegal(x, y)) {

        if(!chessboard[x][y]) {

            undo->setDisabled(false);

            //倒计时
            delete timer;
            timer = new QTimer();
            Timer();

            px = (x + 1) * 40;
            py = (y + 1) * 40;

            if(totalcnt > 250 * listcnt) {
                listcnt++;
                Point *temp = new Point[listcnt * 250];
                for(int i = 0; i < totalcnt; i++) {
                    temp[i] = list[i];
                }
                delete list;
                list = temp;
            }
            stk.push(totalcnt);   //入栈
            list[totalcnt++].setP(x, y, px, py, 1);


            chessboard[x][y] = player;
            scoreboard[x][y][0] = -10;
            update();

            if(JudgeWin(x, y)) {
                timer->stop();
                setEnabled(false);
                QString ss;
                if(player == 1) {
                    ss = "游戏结束，黑方胜！";
                } else {
                    ss = "游戏结束，白方胜！";
                }
                subWin = new QDialog();
                QPushButton *btn = new QPushButton(subWin);
                QLabel *infolabel = new QLabel(subWin);

                infolabel->setText(ss);
                infolabel->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
                infolabel->setGeometry(55, 15, 142, 40);
                infolabel->show();

                btn->setText("OK");
                btn->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
                btn->setGeometry(85, 60, 80, 30);
                btn->show();

                subWin->setWindowFlags(Qt::WindowStaysOnTopHint);
                subWin->setFixedSize(250, 120);
                subWin->setWindowTitle("对局结束");

                subWin->show();

                connect(btn, SIGNAL(clicked(bool)), this, SLOT(Exit()));

                return;
            }

            //判断是否平局
            full = 1;  //先设棋盘已满
            for(int i = 0; i < 15; i++) {
                for(int j = 0; j < 15; j++) {
                    if(!chessboard[i][j]) {
                        full = 0; //棋盘未满
                        break;
                    }
                }
                if(full == 0) {
                    break;
                }
            }
            if(full == 1) {  //若棋盘已满
                update();
                timer->stop();
                setEnabled(false);

                subWin = new QDialog();
                QPushButton *btn = new QPushButton(subWin);
                QLabel *infolabel = new QLabel(subWin);

                infolabel->setText("平局");
                infolabel->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
                infolabel->setGeometry(80, 25, 142, 40);
                infolabel->show();

                btn->setText("OK");
                btn->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
                btn->setGeometry(110, 80, 80, 30);
                btn->show();

                subWin->setWindowFlags(Qt::WindowStaysOnTopHint);
                subWin->setFixedSize(300, 150);
                subWin->setWindowTitle("对局结束");

                subWin->show();

                connect(btn, SIGNAL(clicked(bool)), this, SLOT(Exit()));

                return;

               // QMessageBox::information(this, "Win", "平局！", QMessageBox::Ok);
            }

            Score(x, y);

            //玩家下棋后1s电脑下棋、延时
            QTime dieTime = QTime::currentTime().addMSecs(1000);
            while(QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            AIPlay();
        }

    }

    update();

}

//改变鼠标状态
void AiUpdate::mouseMoveEvent(QMouseEvent *event)
{
    if(event->x() >=25 && event->x() <= 615 &&
            event->y() >= 25 && event->y() <= 615 &&
            chessboard[(event->x() - 25) / 40][(event->y() - 25) / 40]) {
        this->setCursor(Qt::ForbiddenCursor);
    } else {
        this->setCursor(Qt::ArrowCursor);
    }
}


void AiUpdate::ChooseColor()
{
     color = new QButtonGroup();

     white = new QRadioButton("白棋", this);
     black = new QRadioButton("黑棋", this);
     white->setGeometry(700, 20, 80, 20);
     black->setGeometry(820, 20, 80, 20);
     white->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
     black->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
     white->show();
     black->show();

     color->addButton(white, 0);
     color->addButton(black, 1);


     order = new QButtonGroup();

     first = new QRadioButton("玩家先手", this);
     last = new QRadioButton("电脑先手", this);
     first->setGeometry(680, 50, 100, 20);
     last->setGeometry(800, 50, 100, 20);
     first->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
     last->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
     first->show();
     last->show();

     order->addButton(first, 0);
     order->addButton(last, 1);

     connect(white, SIGNAL(clicked(bool)), this, SLOT(Choose()));
     connect(black, SIGNAL(clicked(bool)), this, SLOT(Choose()));
     connect(first, SIGNAL(clicked(bool)), this, SLOT(Choose1()));
     connect(last, SIGNAL(clicked(bool)), this, SLOT(Choose1()));

     start = new QPushButton("start", this);
     start->setGeometry(750, 100, 60, 30);
     start->show();
     start->setDisabled(true);
     start->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
     connect(start, SIGNAL(clicked(bool)), this, SLOT(Start()));

}



//计算价值
void AiUpdate::Score(int a, int b)
{
    //剪枝，不需计算整个棋盘，只要重新计算可能被该点影响到的点
    //即只要计算以该点为中心的星射线上的4*8个点
    int newx, newy;
    for(int i = 1; i < 5; i++) {

        //按四个方向进行找点计算

        newx = a - i;
        newy = b - i;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir1(newx, newy);   //左上-右下
            }
        }

        newx = a + i;
        newy = b + i;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir1(newx, newy);
            }
        }

        newx = a + i;
        newy = b - i;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir2(newx, newy);   //右上-左下
            }
        }

        newx = a - i;
        newy = b + i;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir2(newx, newy);
            }
        }

        newx = a;
        newy = b + i;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir3(newx, newy);    //上-下
            }
        }

        newx = a;
        newy = b - i;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir3(newx, newy);
            }
        }

        newx = a + i;
        newy = b;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir4(newx, newy);    //左-右
            }
        }

        newx = a - i;
        newy = b;
        if(isLegal(newx, newy)) {
            if(!chessboard[newx][newy]) {
                dir4(newx, newy);
            }
        }

    }

}

bool AiUpdate::isLegal(int a,int b)
{
    if(a >= 0 && a <= 14 && b >= 0 && b <= 14) {
        return true;
    }
    return false;
}



//左上-右下
void AiUpdate::dir1(int newx, int newy)
{
    int cnt1 = 0, cnt2 = 0;    //该点两侧连子数
    int emp1 = 0, emp2 = 0;    //空子数
    int flag1 = 0, flag2 = 0;   //标记两侧是否是死子

    for(int i = 1; i <= 14; i++) {
        if(flag1 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx - i, newy - i)) {
            if(chessboard[newx - i][newy - i] == 0) {
                if(cnt1 != 0) {
                    break;
                }
                emp1++;
                continue;
            } else {
                if(chessboard[newx - i + 1][newy - i + 1] == 0) {
                    cnt1++;
                    continue;
                }
            }
            if(chessboard[newx - i][newy - i] ==
                    chessboard[newx - i + 1][newy - i  + 1]) {
                cnt1++;    //连子
            } else if(chessboard[newx - i][newy - i] == 0) {  //空子
                break;
            } else {   //死子
                flag1 = 1;
            }
        } else {
            flag1 = 1;
        }
    }


    for(int i = 1; i <= 14; i++) {
        if(flag2 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx + i, newy + i)) {
            if(chessboard[newx + i][newy + i] == 0) {
                if(cnt2 != 0) {
                    break;
                }
                emp2++;
                continue;
            } else {
                if(chessboard[newx + i - 1][newy + i - 1] == 0) {
                    cnt2++;
                    continue;
                }
            }
            if(chessboard[newx + i][newy + i] ==
                    chessboard[newx + i - 1][newy + i  - 1]) {
                cnt2++;    //连子
            } else if(chessboard[newx + i][newy + i] == 0) {  //空子
                break;
            } else {   //死子
                flag2 = 1;
            }
        } else{
            flag2 = 1;
        }
    }

    if(emp1 == 0 && emp2 == 0) {
        if(cnt1 > 0 && cnt2 > 0 && chessboard[newx - 1][newy - 1]
                != chessboard[newx + 1][newy + 1]) {
            if(cnt1 + 1 >= 5 || cnt2 + 1 >= 5) {
                scoreboard[newx][newy][1] = valuelist[0][5];
                if((cnt1 + 1 >= 5 && chessboard[newx - 1][newy -1] == -1 * player) ||
                        (cnt2 + 1 >= 5 && chessboard[newx + 1][newy + 1] ==
                         -1 * player)) {
                    scoreboard[newx][newy][1] = valuelist[0][5] * 10;
                }

            } else {
                if(flag1 == 1) {
                    scoreboard[newx][newy][1] = 0;
                } else {
                    scoreboard[newx][newy][1] = valuelist[1][cnt1 + 1];
                }
                if(flag2 == 1) {
                    scoreboard[newx][newy][1] += 0;
                } else {
                    scoreboard[newx][newy][1] += valuelist[1][cnt2 + 1];
                }
            }
        } else {
            if(cnt1 + cnt2 + 1 >= 5) {
                scoreboard[newx][newy][1] = valuelist[0][5];
                if(chessboard[newx + 1][newy + 1] == -1 * player) {
                    scoreboard[newx][newy][1] = valuelist[0][5] * 10;
                }
            } else {
                if(flag1 == 1 && flag2 == 1) {
                    scoreboard[newx][newy][1] = 0;
                } else if(flag1 == 0 && flag2 == 0) {
                    scoreboard[newx][newy][1] = valuelist[0][cnt1 + cnt2 + 1];
                } else {
                    scoreboard[newx][newy][1] = valuelist[1][cnt1 + cnt2 + 1];
                }
            }

        }

    } else {
        if(flag1 == 1) {
            if(cnt1 + emp1 + emp2 + 1 < 5) {
                scoreboard[newx][newy][1] = 0;
            } else {
                scoreboard[newx][newy][1] = valuelist[1][cnt1 + 1] / (emp1 + 1);
            }
        } else {
            scoreboard[newx][newy][1] = valuelist[0][cnt1 + 1] / (emp1 + 1);
        }

        if(flag2 == 1) {
            if(cnt2 + emp1 + emp2 + 1 < 5) {
                scoreboard[newx][newy][1] += 0;
            } else {
                scoreboard[newx][newy][1] += valuelist[1][cnt2 + 1] / (emp2 + 1);
            }
        } else {
            scoreboard[newx][newy][1] += valuelist[0][cnt2 + 1] / (emp2 + 1);
        }
    }

    scoreboard[newx][newy][0] = 0;
    for(int i = 1; i < 5; i++) {
        scoreboard[newx][newy][0] += scoreboard[newx][newy][i];
    }

}

//右上-左下
void AiUpdate::dir2(int newx, int newy)
{
    int cnt1 = 0, cnt2 = 0;    //该点两侧连子数
    int emp1 = 0, emp2 = 0;    //空子数
    int flag1 = 0, flag2 = 0;   //标记两侧是否是死子

    for(int i = 1; i <= 14; i++) {
        if(flag1 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx + i, newy - i)) {
            if(chessboard[newx + i][newy - i] == 0) {
                if(cnt1 != 0) {
                    break;
                }
                emp1++;
                continue;
            } else {
                if(chessboard[newx + i - 1][newy - i + 1] == 0) {
                    cnt1++;
                    continue;
                }
            }
            if(chessboard[newx + i][newy - i] ==
                    chessboard[newx + i - 1][newy - i  + 1]) {
                cnt1++;    //连子
            } else if(chessboard[newx + i][newy - i] == 0) {  //空子
                break;
            } else {   //死子
                flag1 = 1;
            }
        } else {
            flag1 = 1;
        }
    }


    for(int i = 1; i <= 14; i++) {
        if(flag2 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx - i, newy + i)) {
            if(chessboard[newx - i][newy + i] == 0) {
                if(cnt2 != 0) {
                    break;
                }
                emp2++;
                continue;
            } else {
                if(chessboard[newx - i + 1][newy + i - 1] == 0) {
                    cnt2++;
                    continue;
                }
            }
            if(chessboard[newx - i][newy + i] ==
                    chessboard[newx - i + 1][newy + i  - 1]) {
                cnt2++;    //连子
            } else if(chessboard[newx - i][newy + i] == 0) {  //空子
                break;
            } else {   //死子
                flag2 = 1;
            }
        } else {
            flag2 = 1;
        }
    }

    if(emp1 == 0 && emp2 == 0) {
        if(cnt1 > 0 && cnt2 > 0 && chessboard[newx + 1][newy - 1]
                != chessboard[newx - 1][newy + 1]) {
            if(cnt1 + 1 >= 5 || cnt2 + 1 >= 5) {
                scoreboard[newx][newy][2] = valuelist[0][5];
                if((cnt1 + 1 >= 5 && chessboard[newx + 1][newy -1] == -1 * player) ||
                        (cnt2 + 1 >= 5 && chessboard[newx - 1][newy + 1] ==
                         -1 * player)) {
                    scoreboard[newx][newy][2] = valuelist[0][5] * 10;
                }

            } else {
                if(flag1 == 1) {
                    scoreboard[newx][newy][2] = 0;
                } else {
                    scoreboard[newx][newy][2] = valuelist[1][cnt1 + 1];
                }
                if(flag2 == 1) {
                    scoreboard[newx][newy][2] += 0;
                } else {
                    scoreboard[newx][newy][2] += valuelist[1][cnt2 + 1];
                }
            }
        } else {
            if(cnt1 + cnt2 + 1 >= 5) {
                scoreboard[newx][newy][2] = valuelist[0][5];
                if(chessboard[newx - 1][newy + 1] == -1 * player) {
                    scoreboard[newx][newy][2] = valuelist[0][5] * 10;
                }
            } else {
                if(flag1 == 1 && flag2 == 1) {
                    scoreboard[newx][newy][2] = 0;
                } else if(flag1 == 0 && flag2 == 0) {
                    scoreboard[newx][newy][2] = valuelist[0][cnt1 + cnt2 + 1];
                } else {
                    scoreboard[newx][newy][2] = valuelist[1][cnt1 + cnt2 + 1];
                }
            }

        }

    } else {
        if(flag1 == 1) {
            if(cnt1 + emp1 + emp2 + 1 < 5) {
                scoreboard[newx][newy][2] = 0;
            } else {
                scoreboard[newx][newy][2] = valuelist[1][cnt1 + 1] / (emp1 + 1);
            }
        } else {
            scoreboard[newx][newy][2] = valuelist[0][cnt1 + 1] / (emp1 + 1);
        }

        if(flag2 == 1) {
            if(cnt2 + emp1 + emp2 + 1 < 5) {
                scoreboard[newx][newy][2] += 0;
            } else {
                scoreboard[newx][newy][2] += valuelist[1][cnt2 + 1] / (emp2 + 1);
            }
        } else {
            scoreboard[newx][newy][2] += valuelist[0][cnt2 + 1] / (emp2 + 1);
        }
    }

    scoreboard[newx][newy][0] = 0;
    for(int i = 1; i < 5; i++) {
        scoreboard[newx][newy][0] += scoreboard[newx][newy][i];
    }

}

// 上-下
void AiUpdate::dir3(int newx, int newy)
{
    int cnt1 = 0, cnt2 = 0;    //该点两侧连子数
    int emp1 = 0, emp2 = 0;    //空子数
    int flag1 = 0, flag2 = 0;   //标记两侧是否是死子

    for(int i = 1; i <= 14; i++) {
        if(flag1 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx, newy - i)) {
            if(chessboard[newx][newy - i] == 0) {
                if(cnt1 != 0) {
                    break;
                }
                emp1++;
                continue;
            } else {
                if(chessboard[newx][newy - i + 1] == 0) {
                    cnt1++;
                    continue;
                }
            }
            if(chessboard[newx][newy - i] ==
                    chessboard[newx][newy - i  + 1]) {
                cnt1++;    //连子
            } else if(chessboard[newx][newy - i] == 0) {  //空子
                break;
            } else {   //死子
                flag1 = 1;
            }
        } else {
            flag1 = 1;
        }
    }


    for(int i = 1; i <= 14; i++) {
        if(flag2 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx, newy + i)) {
            if(chessboard[newx][newy + i] == 0) {
                if(cnt2 != 0) {
                    break;
                }
                emp2++;
                continue;
            } else {
                if(chessboard[newx][newy + i - 1] == 0) {
                    cnt2++;
                    continue;
                }
            }
            if(chessboard[newx][newy + i] ==
                    chessboard[newx][newy + i  - 1]) {
                cnt2++;    //连子
            } else if(chessboard[newx][newy + i] == 0) {  //空子
                break;
            } else {   //死子
                flag2 = 1;
            }
        } else {
            flag2 = 1;
        }
    }

    if(emp1 == 0 && emp2 == 0) {
        if(cnt1 > 0 && cnt2 > 0 && chessboard[newx][newy - 1]
                != chessboard[newx][newy + 1]) {
            if(cnt1 + 1 >= 5 || cnt2 + 1 >= 5) {
                scoreboard[newx][newy][3] = valuelist[0][5];
                if((cnt1 + 1 >= 5 && chessboard[newx][newy -1] == -1 * player) ||
                        (cnt2 + 1 >= 5 && chessboard[newx][newy + 1] ==
                         -1 * player)) {
                    scoreboard[newx][newy][3] = valuelist[0][5] * 10;
                }

            } else {
                if(flag1 == 1) {
                    scoreboard[newx][newy][3] = 0;
                } else {
                    scoreboard[newx][newy][3] = valuelist[1][cnt1 + 1];
                }
                if(flag2 == 1) {
                    scoreboard[newx][newy][3] += 0;
                } else {
                    scoreboard[newx][newy][3] += valuelist[1][cnt2 + 1];
                }
            }
        } else {
            if(cnt1 + cnt2 + 1 >= 5) {
                scoreboard[newx][newy][3] = valuelist[0][5];
                if(chessboard[newx][newy + 1] == -1 * player) {
                    scoreboard[newx][newy][3] = valuelist[0][5] * 10;
                }
            } else {
                if(flag1 == 1 && flag2 == 1) {
                    scoreboard[newx][newy][3] = 0;
                } else if(flag1 == 0 && flag2 == 0) {
                    scoreboard[newx][newy][3] = valuelist[0][cnt1 + cnt2 + 1];
                } else {
                    scoreboard[newx][newy][3] = valuelist[1][cnt1 + cnt2 + 1];
                }
            }

        }

    } else {
        if(flag1 == 1) {
            if(cnt1 + emp1 + emp2 + 1< 5) {
                scoreboard[newx][newy][3] = 0;
            } else {
                scoreboard[newx][newy][3] = valuelist[1][cnt1 + 1] / (emp1 + 1);
            }
        } else {
            scoreboard[newx][newy][3] = valuelist[0][cnt1 + 1] / (emp1 + 1);
        }

        if(flag2 == 1) {
            if(cnt2 + emp1 + emp2 + 1 < 5) {
                scoreboard[newx][newy][3] += 0;
            } else {
                scoreboard[newx][newy][3] += valuelist[1][cnt2 + 1] / (emp2 + 1);
            }
        } else {
            scoreboard[newx][newy][3] += valuelist[0][cnt2 + 1] / (emp2 + 1);
        }
    }

    scoreboard[newx][newy][0] = 0;
    for(int i = 1; i < 5; i++) {
        scoreboard[newx][newy][0] += scoreboard[newx][newy][i];
    }

}

//左-右
void AiUpdate::dir4(int newx, int newy)
{
    int cnt1 = 0, cnt2 = 0;    //该点两侧连子数
    int emp1 = 0, emp2 = 0;    //空子数
    int flag1 = 0, flag2 = 0;   //标记两侧是否是死子

    for(int i = 1; i <= 14; i++) {
        if(flag1 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx - i, newy)) {
            if(chessboard[newx - i][newy] == 0) {
                if(cnt1 != 0) {
                    break;
                }
                emp1++;
                continue;
            } else {
                if(chessboard[newx - i + 1][newy] == 0) {
                    cnt1++;
                    continue;
                }
            }
            if(chessboard[newx - i][newy] ==
                    chessboard[newx - i + 1][newy]) {
                cnt1++;    //连子
            } else if(chessboard[newx - i][newy] == 0) {  //空子
                break;
            } else {   //死子
                flag1 = 1;
            }
        } else {
            flag1 = 1;
        }
    }


    for(int i = 1; i <= 14; i++) {
        if(flag2 != 0) {   //死子或无子
            break;
        }
        if(isLegal(newx + i, newy)) {
            if(chessboard[newx + i][newy] == 0) {
                if(cnt2 != 0) {
                    break;
                }
                emp2++;
                continue;
            } else {
                if(chessboard[newx + i - 1][newy] == 0) {
                    cnt2++;
                    continue;
                }
            }
            if(chessboard[newx + i][newy] ==
                    chessboard[newx + i - 1][newy]) {
                cnt2++;    //连子
            } else if(chessboard[newx + i][newy] == 0) {  //空子
                break;
            } else {   //死子
                flag2 = 1;
            }
        } else {
            flag2 = 1;
        }
    }

    if(emp1 == 0 && emp2 == 0) {
        if(cnt1 > 0 && cnt2 > 0 && chessboard[newx - 1][newy]
                != chessboard[newx + 1][newy]) {
            if(cnt1 + 1 >= 5 || cnt2 + 1 >= 5) {
                scoreboard[newx][newy][4] = valuelist[0][5];
                if((cnt1 + 1 >= 5 && chessboard[newx - 1][newy] == -1 * player) ||
                        (cnt2 + 1 >= 5 && chessboard[newx + 1][newy] ==
                         -1 * player)) {
                    scoreboard[newx][newy][4] = valuelist[0][5] * 10;
                }

            } else {
                if(flag1 == 1) {
                    scoreboard[newx][newy][4] = 0;
                } else {
                    scoreboard[newx][newy][4] = valuelist[1][cnt1 + 1];
                }
                if(flag2 == 1) {
                    scoreboard[newx][newy][4] += 0;
                } else {
                    scoreboard[newx][newy][4] += valuelist[1][cnt2 + 1];
                }
            }
        } else {
            if(cnt1 + cnt2 + 1 >= 5) {
                scoreboard[newx][newy][4] = valuelist[0][5];
                if(chessboard[newx + 1][newy] == -1 * player) {
                    scoreboard[newx][newy][4] = valuelist[0][5] * 10;
                }
            } else {
                if(flag1 == 1 && flag2 == 1) {
                    scoreboard[newx][newy][4] = 0;
                } else if(flag1 == 0 && flag2 == 0) {
                    scoreboard[newx][newy][4] = valuelist[0][cnt1 + cnt2 + 1];
                } else {
                    scoreboard[newx][newy][4] = valuelist[1][cnt1 + cnt2 + 1];
                }
            }

        }

    } else {
        if(flag1 == 1) {
            if(cnt1 + emp1 + emp2 + 1 < 5) {
                scoreboard[newx][newy][4] = 0;
            } else {
                scoreboard[newx][newy][4] = valuelist[1][cnt1 + 1] / (emp1 + 1);
            }
        } else {
            scoreboard[newx][newy][4] = valuelist[0][cnt1 + 1] / (emp1 + 1);
        }

        if(flag2 == 1) {
            if(cnt2 + emp1 + emp2 + 1 < 5) {
                scoreboard[newx][newy][4] += 0;
            } else {
                scoreboard[newx][newy][4] += valuelist[1][cnt2 + 1] / (emp2 + 1);
            }
        } else {
            scoreboard[newx][newy][4] += valuelist[0][cnt2 + 1] / (emp2 + 1);
        }
    }

    scoreboard[newx][newy][0] = 0;
    for(int i = 1; i < 5; i++) {
        scoreboard[newx][newy][0] += scoreboard[newx][newy][i];
    }

}


void AiUpdate::AIPlay()
{
    undo->setDisabled(false);

    //倒计时
    delete timer;
    timer = new QTimer();
    Timer();

    int tempx = 0, tempy = 0;
    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            if( scoreboard[tempx][tempy][0] <= scoreboard[i][j][0]) {
                tempx = i;
                tempy = j;
            }
        }
    }
    chessboard[tempx][tempy] = -1 * player;
    scoreboard[tempx][tempy][0]= -10;

    px = (tempx + 1) * 40;
    py = (tempy + 1) * 40;

    if(totalcnt > 250 * listcnt) {
        listcnt++;
        Point *temp = new Point[listcnt * 250];
        for(int i = 0; i < totalcnt; i++) {
            temp[i] = list[i];
        }
        delete list;
        list = temp;
    }
    stk.push(totalcnt);   //入栈
    list[totalcnt++].setP(tempx, tempy, px, py, -1);

    update();

    if(JudgeWin(tempx, tempy)) {
        timer->stop();
        setEnabled(false);
        QString ss;
        if(player == -1) {
            ss = "游戏结束，黑方胜！";
        } else {
            ss = "游戏结束，白方胜！";
        }
        subWin = new QDialog();
        QPushButton *btn = new QPushButton(subWin);
        QLabel *infolabel = new QLabel(subWin);

        infolabel->setText(ss);
        infolabel->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
        infolabel->setGeometry(55, 15, 142, 40);
        infolabel->show();

        btn->setText("OK");
        btn->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
        btn->setGeometry(85, 60, 80, 30);
        btn->show();

        subWin->setWindowFlags(Qt::WindowStaysOnTopHint);
        subWin->setFixedSize(250, 120);
        subWin->setWindowTitle("对局结束");

        subWin->show();

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(Exit()));

    }

    //判断是否平局
    full = 1;  //先设棋盘已满
    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            if(!chessboard[i][j]) {
                full = 0; //棋盘未满
                break;
            }
        }
        if(full == 0) {
            break;
        }
    }
    if(full == 1) {  //若棋盘已满
        update();
        timer->stop();
        setEnabled(false);

        subWin = new QDialog();
        QPushButton *btn = new QPushButton(subWin);
        QLabel *infolabel = new QLabel(subWin);

        infolabel->setText("平局");
        infolabel->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
        infolabel->setGeometry(80, 25, 142, 40);
        infolabel->show();

        btn->setText("OK");
        btn->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
        btn->setGeometry(110, 80, 80, 30);
        btn->show();

        subWin->setWindowFlags(Qt::WindowStaysOnTopHint);
        subWin->setFixedSize(300, 150);
        subWin->setWindowTitle("对局结束");

        subWin->show();

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(Exit()));

        return;

       // QMessageBox::information(this, "Win", "平局！", QMessageBox::Ok);
    }

    Score(tempx, tempy);

}

//判断是否五子连棋
bool AiUpdate::JudgeWin(int x, int y)
{
    int cnt = 1;
    firx = x;
    firy = y;
    lasx = x;
    lasy = y;

    //判断纵向
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x, y - i)) {
            continue;
        }
        if(chessboard[x][y - i] == chessboard[x][y]) {
            cnt++;
            firx = x;
            firy = y - i;

        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x, y + i)) {
            continue;
        }
        if(chessboard[x][y + i] == chessboard[x][y]) {
            cnt++;
            lasx = x;
            lasy = y + i;
        } else {
            break;
        }
    }
    if(cnt >= 5) {
        firx = (firx + 1) * 40;
        firy = (firy + 1) * 40;
        lasx = (lasx + 1) * 40;
        lasy = (lasy + 1) * 40;
        return true;
    } else {
        cnt = 1;
    }

    //判断横向
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x - i, y)) {
            continue;
        }
        if(chessboard[x - i][y] == chessboard[x][y]) {
            cnt++;
            firx = x - i;
            firy = y;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x + i, y)) {
            continue;
        }
        if(chessboard[x + i][y] == chessboard[x][y]) {
            cnt++;
            lasx = x + i;
            lasy = y;
        } else {
            break;
        }
    }
    if(cnt >= 5) {
        firx = (firx + 1) * 40;
        firy = (firy + 1) * 40;
        lasx = (lasx + 1) * 40;
        lasy = (lasy + 1) * 40;
        return true;
    } else {
        cnt = 1;
    }


    //判断右上——左下
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x + i, y - i)) {
            continue;
        }
        if(chessboard[x + i][y - i] == chessboard[x][y]) {
            cnt++;
            firx = x + i;
            firy = y - i;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x - i, y + i)) {
            continue;
        }
        if(chessboard[x - i][y + i] == chessboard[x][y]) {
            cnt++;
            lasx = x - i;
            lasy = y + i;
        } else {
            break;
        }
    }
    if(cnt >= 5) {
        firx = (firx + 1) * 40;
        firy = (firy + 1) * 40;
        lasx = (lasx + 1) * 40;
        lasy = (lasy + 1) * 40;
        return true;
    } else {
        cnt = 1;
    }


    //判断左上——右下
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x - i, y - i)) {
            continue;
        }
        if(chessboard[x - i][y - i] == chessboard[x][y]) {
            cnt++;
            firx = x - i;
            firy = y - i;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x + i, y + i)) {
            continue;
        }
        if(chessboard[x + i][y + i] == chessboard[x][y]) {
            cnt++;
            lasx = x + i;
            lasy = y + i;
        } else {
            break;
        }
    }
    if(cnt >= 5) {
        firx = (firx + 1) * 40;
        firy = (firy + 1) * 40;
        lasx = (lasx + 1) * 40;
        lasy = (lasy + 1) * 40;
        return true;
    } else {
        cnt = 1;
    }

    //没有五子连珠
    return false;
}


//计时器
void AiUpdate::Timer()
{
    countdown = 30;  //时间重置

    QPalette lcdpat = lcdNumber->palette();
    lcdpat.setColor(QPalette::Normal,QPalette::WindowText,Qt::black);
    lcdNumber->setPalette(lcdpat);

    timecounter = 0;

    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(CountDown()));
    timer->setInterval(10);  //每10ms发射一个timeout信号

}

//倒计时
void AiUpdate::CountDown()
 {

    timecounter += 10;
    QString strTime = QString::number(countdown);
    lcdNumber->display(strTime);  //显示时间

    QPalette lcdpat = lcdNumber->palette();

    if(countdown > 5) {
        if(timecounter < 1000) {
            return ;
        }
    }

    if(countdown <= 5) {

        if(timecounter == 10) {
            QSound::play(":/countdown.wav");
        }

        //闪烁频率0.2s
        if(timecounter % 200 != 0) {
            if(timecounter < 1000) {
                return ;
            }
        } else {
            if(timecounter  % 400 == 0) {
                lcdpat.setColor(QPalette::Normal,QPalette::WindowText,Qt::black);
                lcdNumber->setPalette(lcdpat);
            } else {
                lcdpat.setColor(QPalette::Normal,QPalette::WindowText,Qt::red);
                lcdNumber->setPalette(lcdpat);
            }

        }

        if(timecounter < 1000) {
            return ;
        }
    }

    if(countdown != 0) {
        countdown -= 1;
    } else {
        //timer->stop();
        countdown = 30;
        lcdpat.setColor(QPalette::Normal,QPalette::WindowText,Qt::black);
        lcdNumber->setPalette(lcdpat);
        AIPlay();
       // Timer();
    }

    timecounter = 0;

}


void AiUpdate::Choose()
{
    colorchosen = 1;
    if(colorchosen == 1 && orderchosen == 1) {
        start->setDisabled(false);
    }
}

void AiUpdate::Choose1()
{
    orderchosen = 1;
    if(colorchosen == 1 && orderchosen == 1) {
        start->setDisabled(false);
    }
}



void AiUpdate::Start()
{
    delete start;
    white->setDisabled(true);
    black->setDisabled(true);
    first->setDisabled(true);
    last->setDisabled(true);

    flag = 1;

    if(color->checkedId() == 0) {
        player = -1;
    } else {
        player = 1;
    }

    label = new QLabel(this);
    label->setText("行棋倒计时");
    label->setGeometry(700, 140, 100, 30);
    label->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    label->show();

    lcdNumber = new QLCDNumber(this);
    lcdNumber->setGeometry(820, 140, 60, 30);
    lcdNumber->setDigitCount(2);
    lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    lcdNumber->show();

    timer = new QTimer();
    Timer();

    undo = new QPushButton(this);
    undo->setText("悔棋");
    undo->setGeometry(750, 300, 80, 30);
    undo->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    undo->setDisabled(true);
    undo->show();
    connect(undo, SIGNAL(clicked(bool)), this, SLOT(Undo()));

    if(order->checkedId() == 1) {
        //玩家下棋后1s电脑下棋、延时
        QTime dieTime = QTime::currentTime().addMSecs(1000);
        while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        AIPlay();
    }

    //qDebug() << QString::number(color->checkedId()) << endl;
}

void AiUpdate::Undo()
{
    int p1, p2, tempx1, tempy1, tempx2, tempy2;
    p1 = stk.peek();
    p2 = stk.peek2();

    tempx1 = list[p1].getX();
    tempy1 = list[p1].getY();
    chessboard[tempx1][tempy1] = 0;
    Score(tempx1, tempy1);
    stk.pop();

    if(list[p1].getColor() != list[p2].getColor()) {
        tempx2 = list[p2].getX();
        tempy2 = list[p2].getY();
        chessboard[tempx2][tempy2] = 0;
        Score(tempx2, tempy2);
        stk.pop();
    }

    countdown = 30;
    //chessboard[][]

    update();

}

//点击OK退出子窗口
void AiUpdate::Exit()
{
    subWin->close();
    setEnabled(true);

    clear = new QPushButton(this);
    clear->setText("再来一局");
    clear->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    clear->setGeometry(695, 400, 100, 30);
    clear->show();

    undo->setEnabled(false);
    flag = 0;


    connect(clear, SIGNAL(clicked(bool)), this, SLOT(Again()));

    back = new QPushButton(this);
    back->setText("返回");
    back->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    back->setGeometry(835, 400, 60, 30);
    back->show();
    connect(back, SIGNAL(clicked(bool)), this, SLOT(onBackClicked()));
}

//再玩一局
void AiUpdate::Again()
{
    /*
    PlayWithSelf *again = new PlayWithSelf();
    again->show();
    this->close();
*/
    full = 1;
    wx = -20;    wy = -20;
    bx = -20;    by = -20;
    firx = 0;    firy = 0;
    lasx = 0;    lasy = 0;

    memset(chessboard, 0, sizeof(chessboard));
    memset(scoreboard, 0, sizeof(scoreboard));
    player = 1;
    flag = 0;
    initValue();
    colorchosen = 0;
    orderchosen = 0;

    ChooseColor();


    delete timer;
    delete label;
    delete lcdNumber;
    delete undo;
    delete clear;
    delete back;
    stk.clear();
    delete list;

    update();

}

void AiUpdate::onBackClicked()
{
    this->close();
    MainWindow *m = new MainWindow();
    m->show();
}


#include "aiupdate.h"
#include "ui_aiupdate.h"
#include <QPainter>

AiUpdate::AiUpdate(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AiUpdate)
{
    ui->setupUi(this);
    setFixedSize(970, 640);
    memset(chessboard, 0, sizeof(chessboard));
    memset(scoreboard, 0, sizeof(scoreboard));
    player = 1;
    flag = 0;
    initValue();

    ChooseColor();

}

AiUpdate::~AiUpdate()
{
    delete ui;
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

    /*初始化结果如下  第0列无用
    valuelist = { { 0, 10, 100, 1000, 10000, 100000 },    //活子
                  { 0, 0,  10,  100,  1000,  100000} };   //死子

    */
}

void AiUpdate::paintEvent(QPaintEvent *)  //绘制棋盘
{
    QPainter p(this);


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

}

void AiUpdate::mouseReleaseEvent(QMouseEvent *e)
{
    if(flag == 0) {
        return;
    }

    this->setMouseTracking(true);
    mouseMoveEvent(e);


    if(e->x() >= 25 && e->x() <= 615 && e->y() >= 25 && e->y() <= 615) {

        x = (e->x() - 25) / 40;  //棋点横坐标
        y = (e->y() - 25) / 40;  //棋点纵坐标

        if(!chessboard[x][y]) {
            chessboard[x][y] = player;
            scoreboard[x][y][0] = -10;
            update();

            if(JudgeWin(x, y)) {
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

            Score(x, y);
        }
    }


    //玩家下棋后1s电脑下棋、延时
    QTime dieTime = QTime::currentTime().addMSecs(1000);
    while(QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    AIPlay();


}


void AiUpdate::ChooseColor()
{
     color = new QButtonGroup();

     white = new QRadioButton("white", this);
     black = new QRadioButton("black", this);
     white->setGeometry(700, 20, 80, 20);
     black->setGeometry(820, 20, 80, 20);
     white->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
     black->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
     white->show();
     black->show();

     color->addButton(white, 0);
     color->addButton(black, 1);

     connect(white, SIGNAL(clicked(bool)), this, SLOT(Choose()));
     connect(black, SIGNAL(clicked(bool)), this, SLOT(Choose()));

     start = new QPushButton("start", this);
     start->setGeometry(750, 80, 60, 30);
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
    for(int i = 1; i < 15; i++) {

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

    update();

    if(JudgeWin(tempx, tempy)) {
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

    Score(tempx, tempy);

}

//判断是否五子连棋
bool AiUpdate::JudgeWin(int x, int y)
{
    int cnt = 1;

    //判断纵向
    for(int i = 1; i < 5; i++) {
        if(!isLegal(x, y - i)) {
            continue;
        }
        if(chessboard[x][y - i] == chessboard[x][y]) {
            cnt++;
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
        } else {
            break;
        }
    }
    if(cnt >= 5) {
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
        } else {
            break;
        }
    }
    if(cnt >= 5) {
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
        } else {
            break;
        }
    }
    if(cnt >= 5) {
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
        } else {
            break;
        }
    }
    if(cnt >= 5) {
        return true;
    } else {
        cnt = 1;
    }

    //没有五子连珠
    return false;
}


void AiUpdate::Choose()
{
    start->setDisabled(false);
}



void AiUpdate::Start()
{
    delete start;
    white->setDisabled(true);
    black->setDisabled(true);
    flag = 1;

    if(color->checkedId() == 0) {
        player = -1;
    } else {
        player = 1;
    }

    //qDebug() << QString::number(color->checkedId()) << endl;
}

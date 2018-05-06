#include "playwithself.h"
#include "ui_playwithself.h"
#include <QMessageBox>
#include <QSound>
#include <cmath>

PlayWithSelf::PlayWithSelf(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayWithSelf)
{
    flag = 0;
    full = 1;

    //显示倒计时的标签
    lcdNumber = new QLCDNumber(this);
    lcdNumber->setGeometry(800, 200, 60, 30);
    lcdNumber->setDigitCount(2);
    lcdNumber->setSegmentStyle(QLCDNumber::Flat);
   // lcdNumber->display("30");
    lcdNumber->setVisible(false);


    setFixedSize(970, 640);
    memset(chessboard, 0, sizeof(chessboard));
    PlayRandom();


}

//掷骰子按钮
void PlayWithSelf::PlayRandom()
{
    //ui->setupUi(this);



    button = new QPushButton(this);
    button->setText("Play");
    button->setGeometry(740, 270, 100, 40);
    button->show();

    //掷骰子前预显示图片
    label = new QLabel(this);
    label->setText("null");
    label->setGeometry(690, 50, 200, 200);
    label->setPixmap(QPixmap(":/6.png"));
    label->show();
    label->setAlignment(Qt::AlignCenter);  //居中显示


    //掷骰子
    connect(button, SIGNAL(clicked(bool)), this, SLOT(ShowRandom()));

}

PlayWithSelf::~PlayWithSelf()
{
    //delete ui;
}

void PlayWithSelf::paintEvent(QPaintEvent *)  //绘制棋盘
{
    QPainter p(this);
    p.drawPixmap(0, 0, 970, 640, QPixmap(":/background.jpg"));
    p.setRenderHint(QPainter::Antialiasing, true); //消除锯齿

    for(int i = 1; i < 16; i++) {
        p.drawLine(40, i * 40, 600, i * 40); //画横线,始末点确定线段
        p.drawLine(i * 40, 40, i * 40, 600); //画纵线，始末点确定线段
    }

    //画外框，线条加粗
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

    if(flag == 0) {  //未掷过骰子
        return;
    }

    //最后落子定位
    p.setPen(QPen(Qt::red));
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

//落子
void PlayWithSelf::mouseReleaseEvent(QMouseEvent *e)
{
    if(flag == 0) {  //未掷过骰子
        return;
    }

    this->setMouseTracking(true);
    mouseMoveEvent(e);

    int x, y; //鼠标对应的二维坐标
    if(e->x() >= 25 && e->x() <= 615 && e->y() >= 25 && e->y() <= 615) {

        x = (e->x() - 25) / 40;  //棋点横坐标
        y = (e->y() - 25) / 40;  //棋点纵坐标

        if(!chessboard[x][y]) {

            //倒计时
            delete timer;
            timer = new QTimer();
            Timer();

            //显示当前行棋方
            if(player == -1) {
                info2->setText("当前行棋方:       黑");
                wx = (x + 1) * 40;
                wy = (y + 1) * 40;
            } else {
                info2->setText("当前行棋方:       白");
                bx = (x + 1) * 40;
                by = (y + 1) * 40;
            }

            chessboard[x][y] = player;

            if(JudgeWin(x, y)) {  //游戏结束
                update();
                timer->stop();
                setEnabled(false);
                QString ss;  //胜方信息
                if(player == 1) {
                    ss = "黑方胜！";
                } else {
                    ss= "白方胜！";
                }

                //胜方信息
                QMessageBox::information(this, "Win", ss, QMessageBox::Ok);
            }

            if(player == 1) {    //本次着黑棋
                player = -1;    //下次着白棋
            } else {
                player = 1;  //下次着黑棋
            }

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
            QMessageBox::information(this, "Win", "平局！", QMessageBox::Ok);
        }

    }

    update();  //更新

}

//判断该点是否已有棋子，从而改变鼠标状态
void PlayWithSelf::mouseMoveEvent(QMouseEvent *event)
{
    if(event->x() >=25 && event->x() <= 615 &&
            event->y() >= 25 && event->y() <= 615 &&
            chessboard[(event->x() - 25) / 40][(event->y() - 25) / 40]) {
        this->setCursor(Qt::ForbiddenCursor);
    } else {
        this->setCursor(Qt::ArrowCursor);
    }
}

//判断是否五子连棋
bool PlayWithSelf::JudgeWin(int x, int y)
{
    int cnt = 1;
    firx = x;
    firy = y;
    lasx = x;
    lasy = y;

    //判断横向
    for(int i = 1; i < 5; i++) {
        if(chessboard[x][y - i] == chessboard[x][y]) {
            cnt++;
            firx = x;
            firy = y - i;

        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
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

    //判断纵向
    for(int i = 1; i < 5; i++) {
        if(chessboard[x - i][y] == chessboard[x][y]) {
            cnt++;
            firx = x - i;
            firy = y;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
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
        if(chessboard[x + i][y - i] == chessboard[x][y]) {
            cnt++;
            firx = x + i;
            firy = y - i;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
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
        if(chessboard[x - i][y - i] == chessboard[x][y]) {
            cnt++;
            firx = x - i;
            firy = y - i;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
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

//显示游戏时信息
void PlayWithSelf::ShowStatus()
{

    if(player == 1) {
        info2->setText("当前行棋方:       黑");
    } else {
        info2->setText("当前行棋方:       白");
    }
    info2->setGeometry(700, 150, 200, 30);
    info2->setAlignment(Qt::AlignLeft);
    info2->show();
    info2->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));

    //“行棋倒计时”label
    QLabel *info3 = new QLabel(this);
    info3->setText("行棋倒计时: ");
    info3->setGeometry(700, 200, 100, 30);
    info3->setAlignment(Qt::AlignLeft);
    info3->show();
    info3->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));

    lcdNumber->setVisible(true);

    Timer();

}

void PlayWithSelf::Timer()
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

//掷骰子
void PlayWithSelf::ShowRandom() {

    label1 = new QLabel(this);
    label1->setGeometry(700, 350, 70, 30);

    info1 = new QLabel(this);  //显示先手方

    //调用资源图片
    QMovie *movie;
    srand((unsigned)time(NULL));
    int num = rand() % 6 + 1;    // 1~6随机数
    if(num == 1) {
        movie = new QMovie(":/1.gif");
        label1->setText("白方先手");
        player = -1;  //白方先手
        info1->setText("先手方:               白");
    } else if(num == 2) {
        movie = new QMovie(":/2.gif");
        label1->setText("白方先手");
        player = -1;
        info1->setText("先手方:               白");
    } else if(num == 3) {
        movie = new QMovie(":/3.gif");
        label1->setText("白方先手");
        player = -1;
        info1->setText("先手方:               白");
    } else if(num == 4) {
        movie = new QMovie(":/4.gif");
        label1->setText("黑方先手");
        player = 1;
        info1->setText("先手方:               黑");
    } else if(num == 5) {
        movie = new QMovie(":/5.gif");
        label1->setText("黑方先手");
        player = 1;
        info1->setText("先手方:               黑");
    } else if(num == 6) {
        movie = new QMovie(":6.gif");
        label1->setText("黑方先手");
        player = 1;
        info1->setText("先手方:               黑");
    }

    label->setMovie(movie);
    movie->start();

    label->setAlignment(Qt::AlignCenter);  //居中显示
    label->show();

    label1->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));


    //延时1.5s显示label1内容
    QTime dieTime = QTime::currentTime().addMSecs(1350);
    while(QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    label1->show();  //显示谁先手
    button->setDisabled(true); //掷骰子按钮不可点击(只有一次)

    button1 = new QPushButton(this);
    button1->setText("OK");
    button1->setGeometry(810, 350, 70, 30);
    button1->show();

    //button1 点击销毁一些控件

    info1->setGeometry(700, 100, 150, 30);
    info1->setAlignment(Qt::AlignLeft);
    info1->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));

    connect(button1, SIGNAL(clicked(bool)), this, SLOT(StartPlay()));

}

void PlayWithSelf::StartPlay()
{
    delete label;
    delete button;
    delete label1;
    delete button1;

    flag = 1;

    info1->show();

    ShowStatus();  //显示游戏时信息
}

//倒计时
void PlayWithSelf::CountDown()
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
        if(player == 1) {
            player = -1;
            info2->setText("当前行棋方:       白");
        } else {
            player = 1;
            info2->setText("当前行棋方:       黑");
        }
        countdown = 30;
        lcdpat.setColor(QPalette::Normal,QPalette::WindowText,Qt::black);
        lcdNumber->setPalette(lcdpat);
       // Timer();
    }

    timecounter = 0;

 }


#include "playwithself.h"
#include "ui_playwithself.h"
#include <QMessageBox>
#include <QSound>

PlayWithSelf::PlayWithSelf(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayWithSelf)
{
    flag = 0;

    //显示倒计时的标签
    lcdNumber = new QLCDNumber(this);
    lcdNumber->setGeometry(800, 200, 60, 30);
    lcdNumber->setDigitCount(2);
    lcdNumber->setSegmentStyle(QLCDNumber::Flat);
   // lcdNumber->display("30");
    lcdNumber->setVisible(false);



    setFixedSize(1000, 670);
    memset(chessboard, 0, sizeof(chessboard));
    PlayRandom();


}

//掷骰子按钮
void PlayWithSelf::PlayRandom()
{
    //ui->setupUi(this);



    button = new QPushButton(this);
    button->setText("Play");
    button->setGeometry(770, 270, 100, 40);
    button->show();

    //掷骰子前预显示图片
    label = new QLabel(this);
    label->setText("null");
    label->setGeometry(720, 50, 200, 200);
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
    p.setRenderHint(QPainter::Antialiasing, true); //消除锯齿
    for(int i = 0; i < 16; i++) {
        p.drawLine(35, 35 + i * 40, 635, 35 + i * 40); //画横线,始末点确定线段
        p.drawLine(35 + i * 40, 35, 35 + i * 40, 635); //画纵线，始末点确定线段
    }

    QBrush brush;      //创建画刷
    brush.setStyle(Qt::SolidPattern); //填充风格
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 16; j++) {
            if(chessboard[i][j] == 1) {  //该点上是黑子
                brush.setColor(Qt::black);  // 设置画刷颜色为黑
                p.setBrush(brush);  //使用画刷
                //QPoint(x, y)确定圆心，(15, 15)为椭圆横轴和纵轴(圆半径)
                p.drawEllipse(QPoint(i * 40 + 35, j * 40 + 35), 15, 15);
            } else if(chessboard[i][j] == -1) {  //该点上是白子
                brush.setColor(Qt::white);  //设置画刷颜色为白
                p.setBrush(brush);  //使用画刷
                p.drawEllipse(QPoint(i * 40 + 35, j * 40 + 35), 15, 15);
            }
        }
    }
}

//落子
void PlayWithSelf::mouseReleaseEvent(QMouseEvent *e)
{
    if(flag == 0) {  //未掷过骰子
        return;
    }

    delete timer;
    timer = new QTimer();
    Timer();


    //显示当前行棋方
    if(player == -1) {
        info2->setText("当前行棋方:       黑");
    } else {
        info2->setText("当前行棋方:       白");
    }

    this->setMouseTracking(true);
    mouseMoveEvent(e);

    int x, y; //鼠标对应的二维坐标
    if(e->x() >= 20 && e->x() <= 650 && e->y() >= 20 && e->y() <= 650) {
        x = (e->x() - 20) / 40;  //棋点横坐标
        y = (e->y() - 20) / 40;  //棋点纵坐标
        if(!chessboard[x][y]) {
            chessboard[x][y] = player;

            if(JudgeWin(x, y)) {  //游戏结束
                update();
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

    }


    update();  //更新

}

//判断该点是否已有棋子，从而改变鼠标状态
void PlayWithSelf::mouseMoveEvent(QMouseEvent *event)
{
    if(event->x() >=20 && event->x() <= 650 &&
            event->y() >= 20 && event->y() <= 650 &&
            chessboard[(event->x() - 20) / 40][(event->y() - 20) / 40]) {
        this->setCursor(Qt::ForbiddenCursor);
    } else {
        this->setCursor(Qt::ArrowCursor);
    }
}

//判断是否五子连棋
bool PlayWithSelf::JudgeWin(int x, int y)
{
    int cnt = 1;

    //判断横向
    for(int i = 1; i < 5; i++) {
        if(chessboard[x][y - i] == chessboard[x][y]) {
            cnt++;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(chessboard[x][y + i] == chessboard[x][y]) {
            cnt++;
        }
    }
    if(cnt >= 5) {
        return true;
    } else {
        cnt = 1;
    }

    //判断纵向
    for(int i = 1; i < 5; i++) {
        if(chessboard[x - i][y] == chessboard[x][y]) {
            cnt++;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(chessboard[x + i][y] == chessboard[x][y]) {
            cnt++;
        }
    }
    if(cnt >= 5) {
        return true;
    } else {
        cnt = 1;
    }


    //判断右上——左下
    for(int i = 1; i < 5; i++) {
        if(chessboard[x + i][y - i] == chessboard[x][y]) {
            cnt++;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(chessboard[x - i][y + i] == chessboard[x][y]) {
            cnt++;
        }
    }
    if(cnt >= 5) {
        return true;
    } else {
        cnt = 1;
    }


    //判断左上——右下
    for(int i = 1; i < 5; i++) {
        if(chessboard[x - i][y - i] == chessboard[x][y]) {
            cnt++;
        } else {
            break;
        }
    }
    for(int i = 1; i < 5; i++) {
        if(chessboard[x + i][y + i] == chessboard[x][y]) {
            cnt++;
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
    timer->setInterval(10);

     //延时
}

//掷骰子
void PlayWithSelf::ShowRandom() {

    label1 = new QLabel(this);
    label1->setGeometry(730, 350, 70, 30);

    info1 = new QLabel(this);  //显示先手方

    //调用资源图片
    QMovie *movie;
    srand((unsigned)time(NULL));
    int num = rand() % 6 + 1;
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
    button1->setGeometry(840, 350, 70, 30);
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
            QSound::play("/home/chen/Downloads/countdown.wav");
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


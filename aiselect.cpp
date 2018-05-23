#include "aiselect.h"
#include "ui_aiselect.h"
#include <QPushButton>
#include "playwithai.h"
#include "aiupdate.h"

AISelect::AISelect(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AISelect)
{
    ui->setupUi(this);
    QPushButton *button = new QPushButton(this);
    button->setText("Version1");
    button->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    button->setGeometry(720, 230, 110, 40);
    button->show();

    connect(button, SIGNAL(clicked(bool)), this, SLOT(Play()));

    QPushButton *button1 = new QPushButton(this);
    button1->setText("Version2");
    button1->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    button1->setGeometry(720, 330, 110, 40);
    button1->show();

    connect(button1, SIGNAL(clicked(bool)), this, SLOT(Play1()));


    setFixedSize(970, 640);
}

AISelect::~AISelect()
{
    delete ui;
}

void AISelect::paintEvent(QPaintEvent *)  //绘制棋盘
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

}

void AISelect::Play()
{
    PlayWithAI *playwithai = new PlayWithAI();
    playwithai->show();
    this->close();
}

void AISelect::Play1()
{
    AiUpdate *playwithai1 = new AiUpdate();
    playwithai1->show();
    this->close();
}

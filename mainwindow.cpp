#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playwithself.h"
#include "aiselect.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPushButton *button1 = ui->pushButton;
    button1->setText("玩家对弈");
    button1->setGeometry(700, 180, 200, 50);   //(220,150)坐标，长200，宽50
    button1->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    button1->setDisabled(true);
    //button1->setVisible(false);

    QPushButton *button2 = ui->pushButton_2;
    button2->setText("人机对弈");
    button2->setGeometry(700, 280, 200, 50);
    button2->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    button2->setDisabled(false);

    QPushButton *button3 = ui->pushButton_3;
    button3->setText("单人自弈");
    button3->setGeometry(700, 380, 200, 50);
    button3->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    button3->setDefault(false);

    //跳转界面
    connect(button3, SIGNAL(clicked(bool)), this, SLOT(Button1Clicked()));
    connect(button2, SIGNAL(clicked(bool)), this, SLOT(Button2Clicked()));

    ShowLabel();  //规则介绍文本

    setFixedSize(970, 640);
    //resize(640, 640);

}

void MainWindow::paintEvent(QPaintEvent *) {

    QPainter p(this);
    p.drawPixmap(0, 0, 970, 640, QPixmap(":/background.jpg"));
    p.drawLine(635, 90, 635, 550);

}

void MainWindow::ShowLabel() {  //设置规则介绍文本

    QLabel *label = new QLabel(this);

    QString strText = "*************************游戏规则介绍*************************\n\n"
                      "本游戏在五子棋默认规则下，另增以下几条：\n\n"
                      "1.先手权：\n"
                      "    **黑白双方通过掷骰子决定谁先手。掷到1、2、3点白方先手；\n"
                      "      4、5、6点黑方先手；\n"
                      "    **人机模式下可自由选择玩家先手或电脑先手；\n\n"
                      "2.弃权：\n"
                      "    **黑白双方行棋时间均为30s，逾时仍未着棋者视为放弃该回合\n"
                      "      落子权；\n\n"
                      "3.悔棋：\n"
                      "    **可悔棋多步，但会破坏因弃权改变落子顺序\n"
                      "    **人机对弈时应在玩家落子时间内悔棋，且一次收回双方最近一步。\n\n"
                      "***************************************************************\n";


    label->setText(strText);

    //设置标签大小(512, 450)和位置(90, 100)
    label->setGeometry(QRect(90, 100, 512, 450));

    //设置标签内文字格式
    label->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));

}

void MainWindow::Button1Clicked() {

    PlayWithSelf *play = new PlayWithSelf();  //按钮跳转对象
    play->show();
    this->close();    //关闭当前窗口
}

void MainWindow::Button2Clicked() {

    AISelect *play = new AISelect();  //按钮跳转对象
    play->show();
    this->close();    //关闭当前窗口
}

MainWindow::~MainWindow()
{
    delete ui;
}

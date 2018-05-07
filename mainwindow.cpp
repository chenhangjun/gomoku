#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    button2->setDisabled(true);

    QPushButton *button3 = ui->pushButton_3;
    button3->setText("单人自弈");
    button3->setGeometry(700, 380, 200, 50);
    button3->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));
    button3->setDefault(false);

    //跳转界面
    connect(button3, SIGNAL(clicked(bool)), this, SLOT(Button1Clicked()));

    ShowLabel();  //规则介绍文本

    setFixedSize(970, 640);
    //resize(640, 640);

}

void MainWindow::paintEvent(QPaintEvent *) {

    QPainter p(this);
    p.drawPixmap(0, 0, 970, 640, QPixmap(":/background.jpg"));
    p.drawLine(635, 100, 635, 540);

}

void MainWindow::ShowLabel() {  //设置规则介绍文本

    QLabel *label = new QLabel(this);

    QString strText = "lallalalal\n"
                                     "呵呵\n"
                      "sxa\n"
                                     "asdada\n"
                                     "测试一下\n"
                                     "倒计时30秒\n";


    label->setText(strText);

    //设置标签大小(100, 100)和位置(200, 200)
    label->setGeometry(QRect(185, 200, 100, 200));

    //设置标签内文字格式
    label->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 13));

}

void MainWindow::Button1Clicked() {

    play->show();
    this->close();    //隐藏上一窗口
}

void MainWindow::Back()
{
    this->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

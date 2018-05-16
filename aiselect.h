#ifndef AISELECT_H
#define AISELECT_H

#include <QMainWindow>

namespace Ui {
class AISelect;
}

class AISelect : public QMainWindow
{
    Q_OBJECT

public:
    explicit AISelect(QWidget *parent = 0);
    ~AISelect();
    void paintEvent(QPaintEvent *);


private Q_SLOTS:
    void Play();
    void Play1();

private:
    Ui::AISelect *ui;
};

#endif // AISELECT_H

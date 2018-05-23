#ifndef POINT_H
#define POINT_H


class Point
{

public:
    Point();
    void setP(int x, int y, int px, int py, int color);
    int getX();
    int getY();
    int getPX();
    int getPY();
    int getColor();

private:
    int x, y;
    int px, py;
    int color;

};

#endif // POINT_H

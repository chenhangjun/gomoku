#include "point.h"

Point::Point()
{
    x = 0;
    y= 0;
    px = 0;
    py = 0;
    color = 0;
}

void Point::setP(int x, int y, int px, int py, int color)
{
    this->x = x;
    this->y = y;
    this->px = px;
    this->py = py;
    this->color = color;
}

int Point::getX() {
    return x;
}

int Point::getY() {
    return y;
}

int Point::getPX() {
    return px;
}

int Point::getPY() {
    return py;
}

int Point::getColor() {
    return color;
}

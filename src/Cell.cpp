#include "Cell.h"
#include <cmath>
#include <iostream>

Cell::Cell(double X, double Y)
{
    id = counter;
    counter++;
    extern const double SIZE;
    extern const double OUTLINETHICNESS;
    extern const sf::Color EDGE_COLOR;
    extern const sf::Color ALIVE_COLOR;
    status = true;
    x = X;
    y = Y;
    radius = SIZE / 2;
    circleShape.setPosition(sf::Vector2f(x, y));
    circleShape.setFillColor(ALIVE_COLOR);
    circleShape.setRadius(radius - OUTLINETHICNESS / 2);
    circleShape.setOutlineColor(EDGE_COLOR);
    circleShape.setOutlineThickness(OUTLINETHICNESS);
    circleShape.setPointCount(100);
}
sf::CircleShape Cell::getCircleShape()
{
    return circleShape;
}
void Cell::alive()
{
    extern const sf::Color ALIVE_COLOR;
    status = true;
    circleShape.setFillColor(ALIVE_COLOR);
}
void Cell::death()
{
    extern const sf::Color DEAD_COLOR;
    status = false;
    circleShape.setFillColor(DEAD_COLOR);

    if(id==0){
        circleShape.setFillColor(sf::Color(0,0,0,255));
    }
}
Cell &Cell::operator=(Cell cell)
{
    status = cell.getStatus();
    x = cell.getX();
    y = cell.getY();
    circleShape = cell.getCircleShape();
    return *this;
}
bool Cell::getStatus() const
{
    return status;
}
double Cell::getX() const
{
    return x;
}
double Cell::getY() const
{
    return y;
}
double Cell::getRadius() const
{
    return radius;
}
int Cell::getId() const
{
    return id;
}
void Cell::setFillColor(sf::Color color)
{
    circleShape.setFillColor(color);
}

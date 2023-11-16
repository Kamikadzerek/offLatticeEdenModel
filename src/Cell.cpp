//
// Created by szymek on 10/22/23.
//

#include "Cell.h"
#include <iostream>

Cell::Cell(float X, float Y)
{
//    std::cout<<"New cell: x = "<< X <<" y = "<<Y<<std::endl;
    extern const float RADIUS;
    extern const sf::Color EDGE_COLOR;
    extern const sf::Color ALIVE_COLOR;
    status = true;
    x = X;
    y = Y;
    radius = RADIUS;
    circleShape.setPosition(sf::Vector2f(x,y));
    circleShape.setFillColor(ALIVE_COLOR);
    circleShape.setRadius(radius);
    circleShape.setOutlineColor(EDGE_COLOR);
    circleShape.setOutlineThickness(1);

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
    extern const sf::Color BACKGROUND_COLOR;
    status = false;
    circleShape.setFillColor(BACKGROUND_COLOR);
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
float Cell::getX() const
{
    return x;
}
float Cell::getY() const
{
    return y;
}

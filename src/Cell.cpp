#include "Cell.h"

Cell::Cell(float X, float Y)
{
    id = counter;
    counter++;
    extern const float RADIUS;
    extern const float OUTLINETHICNESS;
    extern const sf::Color EDGE_COLOR;
    extern const sf::Color ALIVE_COLOR;
    status = true;
    x = X;
    y = Y;
    radius = RADIUS;
    circleShape.setPosition(sf::Vector2f(x,y));
    circleShape.setFillColor(ALIVE_COLOR);
    circleShape.setRadius(radius-OUTLINETHICNESS/2);
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
float Cell::getRadius() const
{
    return radius;
}
int Cell::getId() const
{
    return id;
}

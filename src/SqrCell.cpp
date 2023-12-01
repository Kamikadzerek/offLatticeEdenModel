#include "SqrCell.h"
SqrCell::SqrCell(float X, float Y)
{
    id = counter;
    counter++;
    extern const float SIZE;
    extern const float OUTLINETHICNESS;
    extern const sf::Color EDGE_COLOR;
    extern const sf::Color ALIVE_COLOR;
    status = true;
    x = X;
    y = Y;
    size = SIZE;
    rectangleShape.setPosition(sf::Vector2f(x,y));
    rectangleShape.setFillColor(ALIVE_COLOR);
    rectangleShape.setSize(sf::Vector2f(size-OUTLINETHICNESS/2,size-OUTLINETHICNESS/2));
    rectangleShape.setOutlineColor(EDGE_COLOR);
    rectangleShape.setOutlineThickness(OUTLINETHICNESS);
}
sf::RectangleShape SqrCell::getRectangleShape()
{
    return rectangleShape;
}
void SqrCell::alive()
{
    extern const sf::Color ALIVE_COLOR;
    status = true;
    rectangleShape.setFillColor(ALIVE_COLOR);
}
void SqrCell::death()
{
    extern const sf::Color DEAD_COLOR;
    status = false;
    rectangleShape.setFillColor(DEAD_COLOR);
}
SqrCell &SqrCell::operator=(SqrCell sqrCell)
{
    status = sqrCell.getStatus();
    x=sqrCell.getX();
    y=sqrCell.getY();
    rectangleShape = sqrCell.getRectangleShape();
    return *this;
}
bool SqrCell::getStatus() const
{
    return status;
}
float SqrCell::getX() const
{
    return x;
}
float SqrCell::getY() const
{
    return y;
}
float SqrCell::getSize() const
{
    return size;
}
int SqrCell::getId() const
{
    return id;
}

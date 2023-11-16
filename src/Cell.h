//
// Created by szymek on 10/22/23.
//
#include "SFML/Graphics.hpp"
class Cell
{
public:
    Cell(float x, float y);

    void alive();
    void death();
    Cell &operator=(Cell cell);
    bool getStatus() const;
    float getX() const;
    float getY() const;

//    sf::ConvexShape Shape();
    sf::CircleShape getCircleShape();
private:
    bool status; //Dead-False, Alive-True
    float x;
    float y;
    float radius;
    sf::CircleShape circleShape;
};
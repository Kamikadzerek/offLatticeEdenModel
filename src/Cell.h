#include "SFML/Graphics.hpp"
class Cell
{
public:
    Cell(double x, double y);
    void alive();
    void death();
    Cell &operator=(Cell cell);
    bool getStatus() const;
    double getX() const;
    double getY() const;
    double getRadius() const;
    int getId() const;
    sf::CircleShape getCircleShape();
    void setFillColor(sf::Color color);
private:
    static inline int counter = 0;
    int id;
    bool status;//Dead-False, Alive-True
    double x;
    double y;
    double radius;
    sf::CircleShape circleShape;
};

#include "SFML/Graphics.hpp"
class SqrCell
{
public:
    SqrCell(float x, float y);

    void alive();
    void death();
    SqrCell &operator=(SqrCell sqrCell);
    bool getStatus() const;
    float getX() const;
    float getY() const;
    float getSize() const;
    int getId() const;

    sf::RectangleShape getRectangleShape();
private:
    static inline int counter = 0;
    int id;
    bool status; //Dead-False, Alive-True
    float x;
    float y;
    float size;
    sf::RectangleShape rectangleShape;
};

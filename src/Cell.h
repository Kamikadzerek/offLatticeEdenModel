#include "SFML/Graphics.hpp"
class Cell
{
public:
    struct adjacentCell
    {
        Cell *cell;
        float angle;
    };
    Cell(float x, float y);
    void alive();
    void death();
    Cell &operator=(Cell cell);
    bool getStatus() const;
    float getX() const;
    float getY() const;
    float getRadius() const;
    int getId() const;
    sf::CircleShape getCircleShape();
    void addAdjacentCell(Cell *cell);
    void removeAdjacentCell(Cell *cell);
    const std::vector<adjacentCell> &getAdjacentCells() const;

private:
    std::vector<adjacentCell> test;
    std::vector<adjacentCell> adjacentCells;
    static inline int counter = 0;
    int id;
    bool status;//Dead-False, Alive-True
    float x;
    float y;
    float radius;
    sf::CircleShape circleShape;
};

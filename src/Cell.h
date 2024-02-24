#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include <iostream>
#include <vector>
extern const double SIZE;
template<typename T>
class Cell
{
public:
    struct adjacentCell
    {
        Cell<T> *cell;
        double angle;
    };

private:
    std::vector<adjacentCell> adjacentCells;
    T drawable;
    static inline int counter = 0;
    int id;
    bool status;
    double x;
    double y;
    double size;

public:
    Cell(double X, double Y)
    {
        id = counter;
        counter++;
        extern const double OUTLINETHICNESS;
        extern const sf::Color EDGE_COLOR;
        extern const sf::Color ALIVE_COLOR;
        status = true;
        x = X;
        y = Y;
        size = SIZE;
        drawable.setPosition(sf::Vector2f(x, y));
        drawable.setFillColor(ALIVE_COLOR);
        drawable.setOutlineColor(EDGE_COLOR);
        drawable.setOutlineThickness(OUTLINETHICNESS);
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            drawable.setSize(sf::Vector2f(size - OUTLINETHICNESS / 2, size - OUTLINETHICNESS / 2));
        }
        else
        {
            drawable.setRadius(SIZE / 2 - OUTLINETHICNESS/2);
            drawable.setPointCount(100);
        }
    }
    Cell(double X, double Y, bool Status)
    {
        id = counter;
        counter++;
        extern const double SIZE;
        extern const double OUTLINETHICNESS;
        extern const sf::Color EDGE_COLOR;
        extern const sf::Color ALIVE_COLOR;
        extern const sf::Color DEAD_COLOR;
        status = Status;
        x = X;
        y = Y;
        size = SIZE;
        drawable.setPosition(sf::Vector2f(x, y));
        drawable.setOutlineColor(EDGE_COLOR);
        drawable.setOutlineThickness(OUTLINETHICNESS);
        if (status)
        {
            drawable.setFillColor(ALIVE_COLOR);
        }
        else
        {
            drawable.setFillColor(DEAD_COLOR);
        }
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            drawable.setSize(sf::Vector2f(size - OUTLINETHICNESS / 2, size - OUTLINETHICNESS / 2));
        }
        else
        {
            drawable.setRadius(size / 2 - OUTLINETHICNESS / 2);
            drawable.setPointCount(100);
        }
    }
    void alive()
    {
        extern const sf::Color ALIVE_COLOR;
        status = true;
        drawable.setFillColor(ALIVE_COLOR);
    }
    void death()
    {
        extern const sf::Color DEAD_COLOR;
        status = false;
        drawable.setFillColor(DEAD_COLOR);
    }
    Cell &operator=(Cell cell)
    {
        status = cell.getStatus();
        x = cell.getX();
        y = cell.getY();
        drawable = cell.getDrawable();
        return *this;
    }
    bool getStatus() const
    {
        return status;
    }
    double getX() const
    {
        return x;
    }
    double getY() const
    {
        return y;
    }
    double getSize() const
    {
        return size;
    }
    int getId() const
    {
        return id;
    }
    T getDrawable() const
    {
        return drawable;
    }
    static void resetCounter()
    {
        counter = 0;
    }
    void setFillColor(sf::Color color)
    {
        drawable.setFillColor(color);
    }
    void addAdjacentCell(adjacentCell cell)
    {
        adjacentCells.push_back(cell);
    }
    std::vector<adjacentCell> getAdjacentCells() const
    {
        return adjacentCells;
    }
};
template<typename T>
std::ostream &operator<<(std::ostream &out, typename std::vector<Cell<T>>::iterator cell)
{
    out << "Cell {id: " << cell->getId() << "; "
        << "x: " << cell->getX() << "; "
        << "y: " << cell->getY() << "; "
        << "status: " << cell->getStatus() << "}";
    return out;
}
template<typename T>
std::ostream &operator<<(std::ostream &out, Cell<T> *cell)
{
    out << "Cell* {id: " << cell->getId() << "; "
        << "x: " << cell->getX() << "; "
        << "y: " << cell->getY() << "; "
        << "status: " << cell->getStatus() << "}";
    return out;
}
template<typename T>
std::ostream &operator<<(std::ostream &out, const Cell<T> &cell)
{
    out << "Cell& {id: " << cell.getId() << "; "
        << "x: " << cell.getX() << "; "
        << "y: " << cell.getY() << "; "
        << "status: " << cell.getStatus() << "}";
    return out;
}

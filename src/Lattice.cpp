#include "Lattice.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <set>
#include <thread>
extern const double WIDTH;
extern const double HEIGHT;
template<typename S>
auto select_random(const S &s, size_t n)
{
    auto it = std::begin(s);
    // 'advance' the iterator n times
    std::advance(it, n);
    return it;
}
Lattice::Lattice()
{
    aliveCellsCounter = 1;
    initialX = WIDTH / 2;
    initialY = HEIGHT / 2;
    iterationCounter = 0;
    cells.emplace_back(initialX, initialY);
}
void Lattice::updateA(int numberOfIteration)
// In version A, a to-be-infected cell is chosen with same probability
// from all uninfected cells adjacent to the cluster.
// Version A is version B with deleting duplicate from possibleCoords.
{
    std::vector<coords> possibleCoords;
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;
        possibleCoords.clear();
        for (Cell<sf::RectangleShape> &cell: cells)
        {
            if (cell.getStatus())
            {
                int tempPossibleCoordsSize = possibleCoords.size();
                for (const displacement &disp: displacements)
                {
                    double x = cell.getX() + disp.dx;
                    double y = cell.getY() + disp.dy;
                    if (!cellIsConflicting(cells, x, y))
                        possibleCoords.emplace_back(x, y);
                }
                if (possibleCoords.size() == tempPossibleCoordsSize)
                {
                    cell.death();
                    aliveCellsCounter--;
                }
            }
        }
        std::set<coords> possibleCoordsSet(possibleCoords.begin(), possibleCoords.end());
        possibleCoords.assign(possibleCoordsSet.begin(), possibleCoordsSet.end());
        //        coords chosenCoords = *select_random(possibleCoordsSet, rand()%possibleCoordsSet.size());
        coords chosenCoords = possibleCoords[rand() % possibleCoords.size()];
        cells.emplace_back(chosenCoords.x, chosenCoords.y);
        aliveCellsCounter++;
    }
}
void Lattice::updateB(int numberOfIteration)
// In version B, an infection path from all possible paths from infected
// to adjacent uninfected cells is chosen with the same probability
// (the original Eden model).
{
    std::vector<coords> possibleCoords;
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;
        possibleCoords.clear();
        for (Cell<sf::RectangleShape> &cell: cells)
        {
            if (cell.getStatus())
            {
                int tempPossibleCoordsSize = possibleCoords.size();
                for (displacement disp: displacements)
                {
                    double x = cell.getX() + disp.dx;
                    double y = cell.getY() + disp.dy;
                    if (!cellIsConflicting(cells, x, y))
                        possibleCoords.emplace_back(x, y);
                }
                if (possibleCoords.size() == tempPossibleCoordsSize)
                {
                    cell.death();
                    aliveCellsCounter--;
                }
            }
        }
        coords chosenCoords = possibleCoords[rand() % possibleCoords.size()];
        cells.emplace_back(chosenCoords.x, chosenCoords.y);
        aliveCellsCounter++;
    }
}
void Lattice::updateC(int numberOfIteration)
// In version C, firstly a boundary cell of the cluster is randomly chosen,
// then an uninfected adjacent cell is randomly chosen to be infected.
{
    for (int i = 0; i < numberOfIteration; i++)
    {
        bool isConflicting = true;
        iterationCounter++;
        Cell<sf::RectangleShape> *randomCell = &cells[rand() % cells.size()];
        while (!randomCell->getStatus())
        {
            randomCell = &cells[rand() % cells.size()];
        }
        for (displacement disp: displacements)
        {
            double x = randomCell->getX() + disp.dx;
            double y = randomCell->getY() + disp.dy;
            if (!cellIsConflicting(cells, x, y))
            {
                cells.emplace_back(x, y);
                aliveCellsCounter++;
                isConflicting = false;
                break;
            }
        }
        if (isConflicting)
        {
            randomCell->death();
            aliveCellsCounter--;
        }
    }
}
void Lattice::clear()
{
    aliveCellsCounter = 0;
    iterationCounter = 0;
    cells.clear();
    cells.emplace_back(initialX, initialY);
}
bool Lattice::cellIsConflicting(const std::vector<Cell<sf::RectangleShape>> &cells, double x, double y)
{
    for (auto cellTemp = cells.end() - 1; cellTemp != cells.begin(); --cellTemp)
    {
        if (std::abs(cellTemp->getX() - x) < SIZE)
            if (std::abs(cellTemp->getY() - y) < SIZE)
                return true;
    }
    return false;
}
int Lattice::getIterationCounter() const
{
    return iterationCounter;
}
int Lattice::getAliveCellsCounter() const
{
    return aliveCellsCounter;
}
const std::vector<Cell<sf::RectangleShape>> &Lattice::getCells() const
{
    return cells;
}
double Lattice::radiusOfFittedEdge(const sf::CircleShape edge)
{
    double x = edge.getPosition().x;
    double y = edge.getPosition().y;
    double bestRadius = 0;
    double bestSumSquares = MAXFLOAT;
    for (double testRadius = 1.; testRadius < WIDTH; testRadius += 1)
    {
        double sumSquares = 0;
        for (const Cell<sf::RectangleShape> &cell: cells)
        {
            if (cell.getStatus())
            {
                double d = std::sqrt(std::pow(std::abs(x - cell.getX() + SIZE / 2), 2) + std::pow(std::abs(y - cell.getY() + SIZE / 2), 2));// dodatek SIZE/2 wynika z tego że współrzędne (x, y) komórki to współrzędne jej lewego górnego rogu
                sumSquares += std::pow(d - testRadius, 2);
            }
        }
        if (sumSquares > bestSumSquares)
        {
            return bestRadius;
        }
        if (sumSquares < bestSumSquares)
        {
            bestRadius = testRadius;
            bestSumSquares = sumSquares;
        }
    }
    return bestRadius;
}
sf::Vector2f Lattice::getCenterOfMass()
{
    double sumX = 0;
    double sumY = 0;
    for (const Cell<sf::RectangleShape> &cell: cells)
    {
        sumX += cell.getX() + SIZE / 2;
        sumY += cell.getY() + SIZE / 2;
    }
    return sf::Vector2f(sumX / cells.size(), sumY / cells.size());
}
sf::CircleShape Lattice::getEstimateEdge()
{
    sf::CircleShape edge;
    edge.setPosition(getCenterOfMass());
    edge.setFillColor(sf::Color(0, 0, 0, 0));
    edge.setOutlineThickness(2);
    edge.setOutlineColor(sf::Color(0, 0, 0, 10));
    edge.setRadius(radiusOfFittedEdge(edge));
    edge.move(-edge.getRadius(), -edge.getRadius());
    return edge;
}

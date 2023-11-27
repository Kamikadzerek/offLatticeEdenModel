#include "Lattice.h"
#include <chrono>
#include <iostream>
#include <random>
const double PI = 3.14159265358;
extern const float WIDTH;
extern const float HEIGHT;
Lattice::Lattice()
{
    aliveCellsCounter = 1;
    extern const int NUMBEROFANGLES;
    for (double angle = 0; angle < 2 * PI; angle += (2 * PI) / NUMBEROFANGLES)
    {
        angles.push_back(angle);
    }
    initialX = WIDTH / 2;
    initialY = HEIGHT / 2;
    iterationCounter = 0;
    cells.emplace_back(initialX, initialY);
}
Lattice::Lattice(float x, float y)
{
    aliveCellsCounter = 1;
    initialX = x;
    initialY = y;
    iterationCounter = 0;
    cells.emplace_back(initialX, initialY);
}
void Lattice::updateA(int numberOfIteration)
{
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;

        srand(std::chrono::system_clock::now().time_since_epoch().count());
        int loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * float(cells.size()));
        Cell *cell = &cells[loc];
        while (!cell->getStatus())
        {
            loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * float(cells.size()));
            cell = &cells[loc];
        }
        if (cell->getStatus())
        {
            bool isConflicting;
            std::shuffle(angles.begin(), angles.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
            for (const double &angle: angles)
            {
                float x = cell->getX() + spawnDistance * cos(angle);
                float y = cell->getY() + spawnDistance * sin(angle);
                isConflicting = cellIsConflicting(cells, x, y);
                if (!isConflicting)
                {
                    cells.emplace_back(x, y);
                    aliveCellsCounter++;
                    break;
                }
            }
            if (isConflicting)
            {
                cell->death();
                aliveCellsCounter--;
            }
        }
    }
}
void Lattice::updateB(int numberOfIteration)
{
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;

        std::vector<path> possiblepaths;
        //        for (Cell cell: cells)
        for (std::vector<Cell>::iterator cell = cells.begin(); cell != cells.end(); ++cell)
        {
            for (double angle: angles)
            {
                if (cell->getStatus())
                {
                    float x = cell->getX() + spawnDistance * cos(angle);
                    float y = cell->getY() + spawnDistance * sin(angle);
                    if (!cellIsConflicting(cells, x, y))
                    {
                        path tempPath{x = x, y = y};
                        possiblepaths.push_back(tempPath);
                    }
                    else
                    {
                        (*cell).death();
                        aliveCellsCounter--;
                    }
                }
            }
        }
        srand(std::chrono::system_clock::now().time_since_epoch().count());
        int loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * float(possiblepaths.size()));
        path path = possiblepaths[loc];
        cells.push_back(Cell(path.x, path.y));
        aliveCellsCounter++;
    }
}
void Lattice::updateC(int numberOfIteration)
{
}
void Lattice::clear()
{
    iterationCounter = 0;
    cells.clear();
    cells.emplace_back(initialX, initialY);
}
bool Lattice::cellIsConflicting(const std::vector<Cell> &cells, float x, float y)
{
    for (const Cell &cellTemp: cells)
    {
        if (pow(std::abs(cellTemp.getX() - x), 2) + pow(std::abs(cellTemp.getY() - y), 2) < pow(2 * cellTemp.getRadius(), 2))
        {
            return true;
        }
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
const std::vector<Cell> &Lattice::getCells() const
{
    return cells;
}

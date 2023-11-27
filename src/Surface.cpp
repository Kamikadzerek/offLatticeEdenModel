#include "Surface.h"
#include <chrono>
#include <random>
const double PI = 3.14159265358;
extern const float WIDTH;
extern const float HEIGHT;
Surface::Surface()
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
void Surface::updateB(int numberOfIteration)
{
    std::vector<path> possiblepaths;
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;
        possiblepaths.clear();
        for (std::vector<Cell>::iterator cell = cells.begin(); cell != cells.end(); ++cell)
        {
            if (cell->getStatus())
            {
                int tempPossiblePathsSize = possiblepaths.size();
                for (double angle: angles)
                {
                    float x = cell->getX() + spawnDistance * cos(angle);
                    float y = cell->getY() + spawnDistance * sin(angle);
                    if (!cellIsConflicting(cells, x, y))
                    {
                        path tempPath{x = x, y = y};
                        possiblepaths.push_back(tempPath);
                    }
                }
                if (possiblepaths.size() == tempPossiblePathsSize)
                {
                    cell->death();
                    aliveCellsCounter--;
                }
            }
        }
        path path = possiblepaths[rand()%possiblepaths.size()];
        cells.push_back(Cell(path.x, path.y));
        aliveCellsCounter++;
    }
}
void Surface::updateC(int numberOfIteration)
{
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;

        Cell *cell = &cells[rand()%cells.size()];
        while (!cell->getStatus())
        {
            cell = &cells[rand()%cells.size()];
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
void Surface::clear()
{
    aliveCellsCounter = 0;
    iterationCounter = 0;
    cells.clear();
    cells.emplace_back(initialX, initialY);
}
bool Surface::cellIsConflicting(const std::vector<Cell> &cells, float x, float y)
{
    for(const Cell &cellTemp: cells)
    {
        if (pow(std::abs(cellTemp.getX() - x), 2) + pow(std::abs(cellTemp.getY() - y), 2) < pow(2 * cellTemp.getRadius(), 2))
        {
            return true;
        }
    }
    return false;
}
int Surface::getIterationCounter() const
{
    return iterationCounter;
}
int Surface::getAliveCellsCounter() const
{
    return aliveCellsCounter;
}
const std::vector<Cell> &Surface::getCells() const
{
    return cells;
}

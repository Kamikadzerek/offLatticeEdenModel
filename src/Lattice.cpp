#include "Lattice.h"
#include <chrono>
#include <iostream>
#include <random>
const double PI = 3.14159265358;
extern const float WIDTH;
extern const float HEIGHT;
Lattice::Lattice()
{
    initialX = WIDTH / 2;
    initialY = HEIGHT / 2;
    iterationCounter = 0;
    cells.emplace_back(initialX, initialY);
    cellsAlive.push_back(&cells.back());
}
Lattice::Lattice(float x, float y)
{
    initialX = x;
    initialY = y;
    iterationCounter = 0;
    cells.emplace_back(initialX, initialY);
    cellsAlive.push_back(&cells.back());
}
void Lattice::updateA(int numberOfIteration)
{
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;

        srand(std::chrono::system_clock::now().time_since_epoch().count());
        int loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * float(cells.size()));
        Cell *cell = &cells[loc];
        while(!cell->getStatus()){
            loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * float(cells.size()));
            cell = &cells[loc];
        }
        //        Cell *cell = &cells[static_cast<int>((rand() / RAND_MAX) * cells.size())];
        //        while (!cell->getStatus())
        //        {
        //            srand(std::chrono::system_clock::now().time_since_epoch().count());
        //            int loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * cells.size());
        //            cell = &cells[loc];
        //        }
        if (cell->getStatus())
        {
            double randAngle = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * PI * 2;
            std::vector<double> angles({randAngle,
                                        randAngle + (PI * (1. / 4.)),
                                        randAngle + (PI * (2. / 4.)),
                                        randAngle + (PI * (3. / 4.)),
                                        randAngle + (PI * (4. / 4.)),
                                        randAngle + (PI * (5. / 4.)),
                                        randAngle + (PI * (6. / 4.)),
                                        randAngle + (PI * (7. / 4.))});
            std::shuffle(angles.begin(), angles.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
            bool isConflicting = false;
            for (double angle: angles)
            {
                isConflicting = false;
                float x = cell->getX() + spawnDistance * cos(angle);
                float y = cell->getY() + spawnDistance * sin(angle);
                for (const Cell &cellTemp: cells)
                {
                    if (pow(std::abs(x - cellTemp.getX()), 2) + pow(std::abs(y - cellTemp.getY()), 2) < pow(2 * cellTemp.getRadius(), 2))
                    {
                        isConflicting = true;
                        break;
                    }
                }
                if (!isConflicting)
                {
                    cells.emplace_back(x, y);
                    cellsAlive.push_back(&cells.back());
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
}
void Lattice::updateC(int numberOfIteration)
{
}
void Lattice::clear()
{
    iterationCounter = 0;
    cells.clear();
    cellsAlive.clear();
    cells.emplace_back(initialX, initialY);
    cellsAlive.push_back(&cells.back());
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
const std::vector<Cell *> &Lattice::getCellsAlive() const
{
    return cellsAlive;
}

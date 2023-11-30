#include "Lattice.h"
#include <chrono>
#include <iostream>
#include <random>
#include <set>
const double PI = 3.14159265358;
extern const float WIDTH;
extern const float HEIGHT;
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
        for (SqrCell &cell: cells)
        {
            if (cell.getStatus())
            {
                int tempPossibleCoordsSize = possibleCoords.size();
                for (displacement disp: displacements)
                {
                    float x = cell.getX() + disp.dx;
                    float y = cell.getY() + disp.dy;
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
        for (SqrCell &cell: cells)
        {
            if (cell.getStatus())
            {
                int tempPossibleCoordsSize = possibleCoords.size();
                for (displacement disp: displacements)
                {
                    float x = cell.getX() + disp.dx;
                    float y = cell.getY() + disp.dy;
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
        SqrCell *randomCell = &cells[rand() % cells.size()];
        while (!randomCell->getStatus())
        {
            randomCell = &cells[rand() % cells.size()];
        }
        for (displacement disp: displacements)
        {
            float x = randomCell->getX() + disp.dx;
            float y = randomCell->getY() + disp.dy;
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
bool Lattice::cellIsConflicting(const std::vector<SqrCell> &cells, float x, float y)
{
    for (auto cellTemp = cells.end()-1; cellTemp != cells.begin(); --cellTemp)
//    for (const SqrCell &cellTemp: cells)
    {
        if ((std::abs(cellTemp->getX() - x) < SIZE) && (std::abs(cellTemp->getY() - y) < SIZE))
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
const std::vector<SqrCell> &Lattice::getCells() const
{
    return cells;
}

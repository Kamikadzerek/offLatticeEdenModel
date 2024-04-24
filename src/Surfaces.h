//
// Created by szymek on 3/21/24.
//

#include "Surface.h"
#ifndef OFFLATICEEDENMODEL_SURFACES_H
#define OFFLATICEEDENMODEL_SURFACES_H

extern int NUMBEROFLAYERS;
class Surfaces
{
private:
    std::vector<Surface<sf::CircleShape>> surfaces;


    int allCellsCounter = 0;
    int aliveCellsCounter = 0;
    int iterationCounter = 0;

public:
    Surfaces()
    {
//        surfaces.reserve(200);
        for (int i = 0; i < NUMBEROFLAYERS; i++)
        {
            if (i != NUMBEROFLAYERS / 2)
            {
                surfaces.emplace_back(true);
            }
            else
            {

                surfaces.emplace_back(false);
            }
        }
    }
    void update(int numberOfIteration)
    {
        //        for (auto &surface: surfaces)
        //       surface.resetColors();
        for (int i = 0; i < numberOfIteration; i++)
        {
            iterationCounter++;
            //            for (auto &surface: surfaces)
            for (auto surface = surfaces.begin(); surface != surfaces.end(); surface++)
            {
                auto coord = surface->circleUpdateC();
                if (coord.x == 0 && coord.y == 0)
                {
                    allCellsCounter++;
                    aliveCellsCounter++;
                }
                else if (coord.x != 9999 && coord.y != 9999 && surfaces.size()>1)
                {
                    auto nextSurface = surfaces.begin();
                    if (surface == surfaces.begin())
                        nextSurface = surface + 1;
                    else if (surface == surfaces.end() - 1)
                        nextSurface = surface - 1;
                    else if (rand() % 2 == 1)
                        nextSurface = surface + 1;
                    else
                        auto nextSurface = surface - 1;
                    if (!(nextSurface->cellIsConflicting(new const CellPrimitive(coord.x, coord.y))))
                    {
                        allCellsCounter++;
                        nextSurface->addCell(coord.x, coord.y);
                    }
                }
                else { aliveCellsCounter--; }
                //jump to next layer
            }
        }
    }
    std::vector<Surface<sf::CircleShape>> &getSurfaces()
    {
        return surfaces;
    }
    int getAllCellsCounter() const
    {
        return allCellsCounter;
    }
    int getAliveCellsCounter() const
    {
        return aliveCellsCounter;
    }
    int getIterationCounter() const
    {
        return iterationCounter;
    }
};


#endif//OFFLATICEEDENMODEL_SURFACES_H

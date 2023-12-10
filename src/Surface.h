#include "Cell.h"
#include "SFML/Graphics/CircleShape.hpp"
#include <chrono>
#include <cmath>
#include <random>
#include <set>
#include <vector>
extern const double WIDTH;
extern const double HEIGHT;
extern const double SIZE;
template<typename T>
class Surface// store all cells
{
private:
    double initialX;
    double initialY;
    int iterationCounter;
    int aliveCellsCounter;
    double spawnDistance = SIZE;//*1.0005;
    std::vector<Cell<T>> cells;
    std::vector<double> angles;
    struct path
    {
        double x;
        double y;
    };
    struct displacement
    {
        double dx, dy;
    };
    struct coords
    {
        double x, y;
        bool operator<(const coords &coord) const
        {
            return (x < coord.x) || ((!(coord.x < x)) && (y < coord.y));
        }
    };
    std::vector<displacement> displacements = {
            {0, SIZE},
            {0, -SIZE},
            {SIZE, 0},
            {SIZE, SIZE},
            {SIZE, -SIZE},
            {-SIZE, 0},
            {-SIZE, SIZE},
            {-SIZE, -SIZE},
    };
    static bool cellIsConflicting(const std::vector<Cell<T>> &cells, double x, double y)
    {
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            for (auto cellTemp = cells.end() - 1; cellTemp != cells.begin(); --cellTemp)
            {
                if (std::abs(cellTemp->getX() - x) < SIZE)
                    if (std::abs(cellTemp->getY() - y) < SIZE)
                        return true;
            }
            return false;
        }
        else
        {
            double radius = cells.begin()->getSize() / 2;
            for (auto cellTemp = cells.end(); cellTemp > cells.begin() - 1; cellTemp--)
            {
                if (std::abs(cellTemp->getX() - x) < 2 * radius)
                {
                    if (distanceBtwTwoPoints(cellTemp->getX(), cellTemp->getY(), x, y) < 2 * radius)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
    }
    static double radiusOfFittedEdge(const sf::CircleShape &edge, const std::vector<Cell<T> *> &edgeCells)
    {
        double x = edge.getPosition().x;
        double y = edge.getPosition().y;
        double bestRadius = 0;
        double bestSumSquares = MAXFLOAT;
        for (double testRadius = 1.; testRadius < WIDTH; testRadius += 1)
        {
            double sumSquares = 0;
            for (const Cell<T> *cell: edgeCells)
            {
                if (cell->getStatus())
                {
                    double d = std::sqrt(std::pow(std::abs(x - cell->getX() + SIZE / 2), 2) + std::pow(std::abs(y - cell->getY() + SIZE / 2), 2));// dodatek SIZE/2 wynika z tego że współrzędne (x, y) komórki to współrzędne jej lewego górnego rogu
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
    void resetColors()
    {
        extern const sf::Color ALIVE_COLOR;
        extern const sf::Color DEAD_COLOR;
        for (auto cell = cells.begin(); cell != cells.end(); cell++)
        {
            if (cell->getStatus())
            {
                cell->setFillColor(ALIVE_COLOR);
            }
            else
            {
                cell->setFillColor(DEAD_COLOR);
            }
        }
    }

public:
    struct adjacentCell
    {
        Cell<T> *cell;
        double angle;
    };
    std::vector<adjacentCell> getAdjacentCells(const Cell<T> *cell)
    {
        std::vector<Surface::adjacentCell> adjacentCells = {};
        for (auto cellTemp = cells.begin(); cellTemp != cells.end(); cellTemp++)
        {
            if (cellTemp->getId() != cell->getId())
            {
                if (distanceBtwTwoPoints(cell->getX(), cell->getY(), cellTemp->getX(), cellTemp->getY()) <= spawnDistance + 1)
                {
                    adjacentCells.push_back(adjacentCell(&(*cellTemp), angleBtwTwoPoints(cell->getX(), cell->getY(), cellTemp->getX(), cellTemp->getY())));
                }
            }
        }
        return adjacentCells;
    }
    void printCellInfoByClick(double x, double y)
    {
        double cellCenterX;
        double cellCenterY;
        for (const Cell<T> &cell: cells)
        {
            cellCenterX = cell.getX() + cell.getSize() / 2;
            cellCenterY = cell.getY() + cell.getSize() / 2;
            if (distanceBtwTwoPoints(cellCenterX, cellCenterY, x, y) < cell.getSize() / 2)
            {

                std::vector<Surface::adjacentCell> adjacentCells = getAdjacentCells(&cell);
                std::cout << cell << "\n";
                //                std::cout << adjacentCells;
                std::cout << "\n--------------------------------------------------------\n";
            }
        }
    }
    sf::Vector2f getCenterOfMass()
    {
        double sumX = 0;
        double sumY = 0;
        for (const Cell<T> &cell: cells)
        {
            sumX += cell.getX();
            sumY += cell.getY();
        }
        return sf::Vector2f(sumX / cells.size(), sumY / cells.size());
    }
    sf::CircleShape getEstimateEdge(const std::vector<Cell<T> *> &edgeCells)
    {
        sf::CircleShape edge;
        edge.setPosition(getCenterOfMass());
        edge.setFillColor(sf::Color(0, 0, 0, 0));
        edge.setOutlineThickness(2);
        edge.setOutlineColor(sf::Color(0, 0, 0, 100));
        edge.setRadius(radiusOfFittedEdge(edge, edgeCells));
        edge.move(-edge.getRadius(), -edge.getRadius());
        return edge;
    }
    std::vector<Cell<T> *> getEdgeCells()
    {
        std::vector<Cell<T> *> edgeCells;
        Cell<T> *farRightCell = nullptr;
        for (Cell<T> &cell: cells)
        {
            if (cell.getStatus())
            {
                if (farRightCell == nullptr)
                    farRightCell = &cell;
                if (cell.getX() > farRightCell->getX())
                    farRightCell = &cell;
            }
        }
        edgeCells.push_back(farRightCell);

        Cell<T> *currentCell = edgeCells.back();
        std::vector<Surface::adjacentCell> adjacentCells = {};
        do {
            adjacentCells.clear();
            adjacentCells = getAdjacentCells(currentCell);
            std::sort(adjacentCells.begin(), adjacentCells.end(), [](Surface::adjacentCell c1, Surface::adjacentCell c2)
                      { return c1.angle < c2.angle; });
            for (auto adjacentCell = adjacentCells.begin(); adjacentCell < adjacentCells.end(); adjacentCell++)
            {
                if (adjacentCells.size() == 1)
                {
                    edgeCells.push_back(adjacentCells.back().cell);
                    currentCell = edgeCells.back();
                    break;
                }
                if (edgeCells.size() < 2)
                {
                    edgeCells.push_back(adjacentCells.begin()->cell);
                    currentCell = edgeCells.back();
                    break;
                }
                if (adjacentCell->cell->getId() == (edgeCells.end()[-2])->getId())
                {
                    if (adjacentCell->cell->getId() == adjacentCells.back().cell->getId())
                    {
                        edgeCells.push_back(adjacentCells.begin()->cell);
                    }
                    else
                    {
                        edgeCells.push_back((adjacentCell + 1)->cell);
                    }
                    currentCell = edgeCells.back();
                    break;
                }
            }

        } while (currentCell != farRightCell);
        return edgeCells;
    }
    int getIterationCounter() const
    {
        return iterationCounter;
    }
    int getAliveCellsCounter() const
    {
        return aliveCellsCounter;
    }
    const std::vector<Cell<T>> &getCells() const
    {
        return cells;
    }
    Surface()
    {
        aliveCellsCounter = 1;
        initialX = WIDTH / 2;
        initialY = HEIGHT / 2;
        iterationCounter = 0;
        if constexpr (std::is_same<T, sf::CircleShape>::value)
        {
            extern const int NUMBEROFANGLES;
            for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / NUMBEROFANGLES)
            {
                angles.push_back(angle);
            }
        }
        cells.emplace_back(initialX, initialY);
    }
    void circleUpdateB(int numberOfIteration)
    {
        resetColors();
        std::vector<path> possiblePaths;
        for (int i = 0; i < numberOfIteration; i++)
        {
            iterationCounter++;
            possiblePaths.clear();
            for (auto &cell: cells)
            {
                if (cell.getStatus())
                {
                    unsigned long tempPossiblePathsSize = possiblePaths.size();
                    for (double angle: angles)
                    {
                        double x = cell.getX() + spawnDistance * double(cos(angle));
                        double y = cell.getY() + spawnDistance * double(sin(angle));
                        if (!cellIsConflicting(cells, x, y))
                        {
                            path tempPath{x = x, y = y};
                            possiblePaths.push_back(tempPath);
                        }
                    }
                    if (possiblePaths.size() == tempPossiblePathsSize)
                    {
                        cell.death();
                        aliveCellsCounter--;
                    }
                }
            }
            if (!possiblePaths.empty())
            {
                path path = possiblePaths[rand() % possiblePaths.size()];
                cells.emplace_back(path.x, path.y);
                aliveCellsCounter++;
            }
        }
    }
    void circleUpdateC(int numberOfIteration)
    {
        resetColors();
        for (int i = 0; i < numberOfIteration; i++)
        {
            iterationCounter++;
            unsigned long index = rand() % cells.size();
            auto *cell = &cells[index];
            while (!cell->getStatus())
            {
                std::cout<<"DUPA\n";
                index = rand() % cells.size();
                cell = &cells[index];
            }
            if (cell->getStatus())
            {
                bool isConflicting;
                std::shuffle(angles.begin(), angles.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
                for (const double &angle: angles)
                {
                    double x = cell->getX() + spawnDistance * cos(angle);
                    double y = cell->getY() + spawnDistance * sin(angle);
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
    void rectangleUpdateA(int numberOfIteration)
    // In version A, a to-be-infected cell is chosen with same probability
    // from all uninfected cells adjacent to the cluster.
    // Version A is version B with deleting duplicate from possibleCoords.
    {
        std::vector<coords> possibleCoords;
        for (int i = 0; i < numberOfIteration; i++)
        {
            iterationCounter++;
            possibleCoords.clear();
            for (auto &cell: cells)
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
    void rectangleUpdateB(int numberOfIteration)
    // In version B, an infection path from all possible paths from infected
    // to adjacent uninfected cells is chosen with the same probability
    // (the original Eden model).
    {
        std::vector<coords> possibleCoords;
        for (int i = 0; i < numberOfIteration; i++)
        {
            iterationCounter++;
            possibleCoords.clear();
            for (auto &cell: cells)
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
    void rectangleUpdateC(int numberOfIteration)
    // In version C, firstly a boundary cell of the cluster is randomly chosen,
    // then an uninfected adjacent cell is randomly chosen to be infected.
    {
        for (int i = 0; i < numberOfIteration; i++)
        {
            bool isConflicting = true;
            iterationCounter++;
            Cell<T> *randomCell = &cells[rand() % cells.size()];
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
    void clear()
    {
        Cell<T>::resetCounter();
        aliveCellsCounter = 0;
        iterationCounter = 0;
        cells.clear();
        cells.emplace_back(initialX, initialY);
    }
    static double distanceBtwTwoPoints(double x1, double y1, double x2, double y2)
    {
        return sqrt(pow(std::abs(x1 - x2), 2) + pow(std::abs(y1 - y2), 2));
    }
    double angleBtwTwoPoints(double x1, double y1, double x2, double y2)
    {
        double angle = atan2((y2 - y1), (x2 - x1));
        if (angle < 0)
        {
            angle += 2 * M_PI;
        }
        return angle;
    }
};
template<typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<typename Surface<T>::adjacentCell> &vecAC)
{
    std::vector<typename Surface<T>::adjacentCell> vAC = vecAC;
    std::sort(vAC.begin(), vAC.end(), [](Surface<T>::adjacentCell c1, Surface<T>::adjacentCell c2)
              { return c1.angle < c2.angle; });
    out << "AdjacentCells:{";
    for (const typename Surface<T>::adjacentCell cell: vAC)
    {
        out << "\n"
            << cell.cell << ", angle: " << cell.angle * (180 / M_PI);
        if (cell.cell->getId() != vAC.back().cell->getId())
        {
            out << ",";
        }
    }
    out << "\n}";
    return out;
}

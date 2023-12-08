#include "Surface.h"
#include "functions.cpp"
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
extern const double WIDTH;
extern const double HEIGHT;
double distanceBtwTwoPoints(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(std::abs(x1 - x2), 2) + pow(std::abs(y1 - y2), 2));
}
double angleBtwTwoPoints(double x1, double y1, double x2, double y2)
{
    double angle = atan2((y2 - y1) , (x2 - x1));
    if(angle<0){
        angle += 2*M_PI;
    }
    return angle;
}
std::ostream &operator<<(std::ostream &out, std::vector<Cell>::iterator cell)
{
    out << "Cell {id: " << cell->getId() << "; "
        << "x: " << cell->getX() << "; "
        << "y: " << cell->getY() << "; "
        << "status: " << cell->getStatus() << "}";
    return out;
}
std::ostream &operator<<(std::ostream &out, Cell *cell)
{
    out << "Cell* {id: " << cell->getId() << "; "
        << "x: " << cell->getX() << "; "
        << "y: " << cell->getY() << "; "
        << "status: " << cell->getStatus() << "}";
    return out;
}
std::ostream &operator<<(std::ostream &out, const std::vector<Surface::adjacentCell> &vecAC)
{
    std::vector<Surface::adjacentCell> vAC = vecAC;
    std::sort(vAC.begin(), vAC.end(),[](Surface::adjacentCell c1, Surface::adjacentCell c2)
              { return c1.angle < c2.angle; });
    out << "AdjacentCells:{";
    for (const Surface::adjacentCell cell: vAC)
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
std::ostream &operator<<(std::ostream &out, const Cell &cell)
{
    out << "Cell& {id: " << cell.getId() << "; "
        << "x: " << cell.getX() << "; "
        << "y: " << cell.getY() << "; "
        << "status: " << cell.getStatus() << "}";
    return out;
}
Surface::Surface()
{
    aliveCellsCounter = 1;
    extern const int NUMBEROFANGLES;
    for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / NUMBEROFANGLES)
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
    resetColors();
    std::vector<path> possiblepaths;
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;
        possiblepaths.clear();
        for (auto &cell: cells)
        {
            if (cell.getStatus())
            {
                unsigned long tempPossiblePathsSize = possiblepaths.size();
                for (double angle: angles)
                {
                    double x = cell.getX() + spawnDistance * double(cos(angle));
                    double y = cell.getY() + spawnDistance * double(sin(angle));
                    if (!cellIsConflicting(cells, x, y))
                    {
                        path tempPath{x = x, y = y};
                        possiblepaths.push_back(tempPath);
                    }
                }
                if (possiblepaths.size() == tempPossiblePathsSize)
                {
                    cell.death();
                    aliveCellsCounter--;
                }
            }
        }
        if (!possiblepaths.empty())
        {
            path path = possiblepaths[rand() % possiblepaths.size()];
            cells.emplace_back(path.x, path.y);
            aliveCellsCounter++;
        }
    }
}
void Surface::updateC(int numberOfIteration)
{
    resetColors();
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;
        unsigned long index = rand() % cells.size();
        Cell *cell = &cells[index];
        while (!cell->getStatus())
        {
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
void Surface::clear()
{
    Cell::resetCounter();
    aliveCellsCounter = 0;
    iterationCounter = 0;
    cells.clear();
    cells.emplace_back(initialX, initialY);
}
bool Surface::cellIsConflicting(const std::vector<Cell> &cells, double x, double y)
{
    double radius = cells.begin()->getRadius();
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
double Surface::radiusOfFittedEdge(const sf::CircleShape& edge, const std::vector<Cell*>& edgeCells)
{
    double x = edge.getPosition().x;
    double y = edge.getPosition().y;
    double bestRadius = 0;
    double bestSumSquares = MAXFLOAT;
    for (double testRadius = 1.; testRadius < WIDTH; testRadius += 1)
    {
        double sumSquares = 0;
        for (const Cell *cell: edgeCells)
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
sf::Vector2f Surface::getCenterOfMass()
{
    double sumX = 0;
    double sumY = 0;
    for (const Cell &cell: cells)
    {
        sumX += cell.getX();
        sumY += cell.getY();
    }
    return sf::Vector2f(sumX / cells.size(), sumY / cells.size());
}
sf::CircleShape Surface::getEstimateEdge(const std::vector<Cell*>& edgeCells)
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
std::vector<Cell *> Surface::getEdgeCells()
{
    std::vector<Cell *> edgeCells;
    Cell *farRightCell = nullptr;
    for (Cell &cell: cells)
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

    Cell *currentCell = edgeCells.back();
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
void Surface::printCellInfoByClick(double x, double y)
{
    double cellCenterX;
    double cellCenterY;
    for (const Cell& cell: cells)
    {
        cellCenterX = cell.getX() + cell.getRadius();
        cellCenterY = cell.getY() + cell.getRadius();
        if (distanceBtwTwoPoints(cellCenterX, cellCenterY, x, y) < cell.getRadius())
        {

            std::vector<Surface::adjacentCell> adjacentCells = getAdjacentCells(&cell);
            std::cout << cell << "\n";
            std::cout << adjacentCells;
            std::cout << "\n--------------------------------------------------------\n";
        }
    }
}
std::vector<Surface::adjacentCell> Surface::getAdjacentCells(const Cell *cell)
{
    std::vector<Surface::adjacentCell> adjacentCells = {};
    for (auto cellTemp = cells.begin(); cellTemp != cells.end(); cellTemp++)
    {
        if (cellTemp->getId()!=cell->getId()){
        if (distanceBtwTwoPoints(cell->getX(), cell->getY(), cellTemp->getX(), cellTemp->getY()) <= spawnDistance+1)
        {
            adjacentCells.push_back(adjacentCell(&(*cellTemp), angleBtwTwoPoints(cell->getX(), cell->getY(), cellTemp->getX(), cellTemp->getY())));
        }}
    }
    return adjacentCells;
}
void Surface::resetColors()
{
    extern const sf::Color ALIVE_COLOR;
    extern const sf::Color DEAD_COLOR;
    for(auto cell = cells.begin();cell!=cells.end();cell++){
        if(cell->getStatus()){
        cell->setFillColor(ALIVE_COLOR);
        }else{
            cell->setFillColor(DEAD_COLOR);

        }

    }
}
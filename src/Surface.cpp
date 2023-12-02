#include "Surface.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
const double PI = 3.14159265358;
extern const float WIDTH;
extern const float HEIGHT;
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
    out << "Cell {id: " << cell->getId() << "; "
        << "x: " << cell->getX() << "; "
        << "y: " << cell->getY() << "; "
        << "status: " << cell->getStatus() << "}";
    return out;
}
std::ostream &operator<<(std::ostream &out, const Cell &cell)
{
    out << "Cell {id: " << cell.getId() << "; "
        << "x: " << cell.getX() << "; "
        << "y: " << cell.getY() << "; "
        << "status: " << cell.getStatus() << "}";
    return out;
}
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
        for (auto &cell: cells)
        {
            if (cell.getStatus())
            {
                int tempPossiblePathsSize = possiblepaths.size();
                for (double angle: angles)
                {
                    float x = cell.getX() + spawnDistance * float(cos(angle));
                    float y = cell.getY() + spawnDistance * float(sin(angle));
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
    for (int i = 0; i < numberOfIteration; i++)
    {
        iterationCounter++;

        Cell *cell = &cells[rand() % cells.size()];
        while (!cell->getStatus())
        {
            cell = &cells[rand() % cells.size()];
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
                    cell->addAdjacentCell(&cells.back());
                    std::cout<<"DUPA\n";
                    cells.end()->addAdjacentCell(cell);
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
    float radius = cells.begin()->getRadius();
    for (auto cellTemp = cells.end() - 1; cellTemp != cells.begin(); cellTemp--)
    {
        if (std::abs(cellTemp->getX() - x) < 2 * radius)
        {
            if (pow(std::abs(cellTemp->getX() - x), 2) + pow(std::abs(cellTemp->getY() - y), 2) < pow(2 * radius, 2))
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
float Surface::radiusOfFittedEdge(const sf::CircleShape edge)
{
    float x = edge.getPosition().x;
    float y = edge.getPosition().y;
    float bestRadius = 0;
    float bestSumSquares = MAXFLOAT;
    for (float testRadius = 1.; testRadius < WIDTH; testRadius += 1)
    {
        float sumSquares = 0;
        for (const Cell &cell: cells)
        {
            if (cell.getStatus())
            {
                float d = std::sqrt(std::pow(std::abs(x - cell.getX() + SIZE / 2), 2) + std::pow(std::abs(y - cell.getY() + SIZE / 2), 2));// dodatek SIZE/2 wynika z tego że współrzędne (x, y) komórki to współrzędne jej lewego górnego rogu
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
    float sumX = 0;
    float sumY = 0;
    for (const Cell &cell: cells)
    {
        sumX += cell.getX();
        sumY += cell.getY();
    }
    return sf::Vector2f(sumX / cells.size(), sumY / cells.size());
}
sf::CircleShape Surface::getEstimateEdge()
{
    sf::CircleShape edge;
    edge.setPosition(getCenterOfMass());
    edge.setFillColor(sf::Color(0, 0, 0, 0));
    edge.setOutlineThickness(2);
    edge.setOutlineColor(sf::Color(0, 0, 0, 255));
    edge.setRadius(radiusOfFittedEdge(edge));
    edge.move(-edge.getRadius(), -edge.getRadius());
    return edge;
}
std::vector<Cell *> Surface::getEdgeCells()
{
    float radius = cells.begin()->getRadius();
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
    Cell *cellptr;
    do {
        cellptr = edgeCells.back();
        for(Cell::adjacentCell adjacentCell: cellptr->getAdjacentCells()){
            std::cout<<adjacentCell.cell<<"\n";
        }
    } while (cellptr != farRightCell);


    return edgeCells;
}
// Sposób znajdowania krawędziowych komórek:
// W wektorze adjacentCell przechowuj pary (komórka;kąt),(komórka;kąt-pi) odpowiednio dla komórki "rodzica" i komórki "potomka". Gdzię kąt to wylosowana zmienna angle
// Następnie za pierwszą komórkę krawędziową przyjmujesz komórkę wysuniętą najbardziej na prawo w klastrze.
// Aby znaleźć następną stosuj algorytm:
// 1. Znajdź kąt prostej łączącej środek komórki poprzedniej ze środkiem ciężkości klastra.
// 2. Posegreguj wektor komórek przylegających komórki poprzedniej po kącię. (optymalnie jeśli za pierwszy uznana byłaby para w której kąt jako pierwszy następuję po kącie znalezionym w poprzednim kroku)
// 3. W sytuacji optymalnej komórka z pierwszej pary będzie szukaną komórką.
// 4. Dodaj ją do komórek krawędziowych i usuń z jej wektora adjacenCell poprzednią komórkę.
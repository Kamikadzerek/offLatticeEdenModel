#include "Cell.h"
#include <vector>
extern const double SIZE;
class Surface// store all cells
{
private:
    double initialX;
    double initialY;
    int iterationCounter;
    int aliveCellsCounter;
    double spawnDistance = SIZE; //*1.0005;
    std::vector<Cell> cells;
    std::vector<double> angles;
    struct path
    {
        double x;
        double y;
    };
    static bool cellIsConflicting(const std::vector<Cell> &cells, double x, double y);
    static double radiusOfFittedEdge(const sf::CircleShape& edge,const std::vector<Cell*>& edgeCells);
    void resetColors();

public:
    struct adjacentCell{
        Cell *cell;
        double angle;
    };
    std::vector<adjacentCell> getAdjacentCells(const Cell* cell);
    void printCellInfoByClick(double x, double y);
    sf::Vector2f getCenterOfMass();
    sf::CircleShape getEstimateEdge(const std::vector<Cell*>& edgeCells);
    std::vector<Cell *> getEdgeCells();
    int getIterationCounter() const;
    int getAliveCellsCounter() const;
    const std::vector<Cell> &getCells() const;
    Surface();
    void updateB(int numberOfIteration);
    void updateC(int numberOfIteration);
    void clear();
};

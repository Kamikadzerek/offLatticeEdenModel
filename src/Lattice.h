#include "SqrCell.h"
#include <valarray>
#include <vector>
extern const double SIZE;
class Lattice
{
private:
    double initialX;
    double initialY;
    int iterationCounter;
    int aliveCellsCounter;
    std::vector<SqrCell> cells;
    struct displacement
    {
        double dx,dy;
    };
    struct coords
    {
        double x,y;
        bool operator <(const coords& coord) const
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
    static bool
    cellIsConflicting(const std::vector<SqrCell> &cells, double x, double y);

public:
    int getIterationCounter() const;
    int getAliveCellsCounter() const;
    const std::vector<SqrCell> &getCells() const;
    Lattice();
    void updateA(int numberOfIteration);
    void updateB(int numberOfIteration);
    void updateC(int numberOfIteration);
    void clear();
    sf::CircleShape getEstimateEdge();
    sf::Vector2f getCenterOfMass();
    double radiusOfFittedEdge(const sf::CircleShape edge);
};
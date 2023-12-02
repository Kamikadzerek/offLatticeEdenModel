#include "SqrCell.h"
#include <valarray>
#include <vector>
extern const float SIZE;
class Lattice
{
private:
    float initialX;
    float initialY;
    int iterationCounter;
    int aliveCellsCounter;
    std::vector<SqrCell> cells;
    struct displacement
    {
        float dx,dy;
    };
    struct coords
    {
        float x,y;
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
    cellIsConflicting(const std::vector<SqrCell> &cells, float x, float y);

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
    float radiusOfFittedEdge(const sf::CircleShape edge);
};
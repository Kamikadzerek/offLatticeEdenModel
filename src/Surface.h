#include "Cell.h"
#include <vector>
extern const float DISTANCE;
class Surface// store all cells
{
private:
    float initialX;
    float initialY;
    int iterationCounter;
    int aliveCellsCounter;
    float spawnDistance = DISTANCE;
    std::vector<Cell> cells;
    std::vector<double> angles;
    struct path{
        float x;
        float y;
    };
    static bool cellIsConflicting(const std::vector<Cell> &cells, float x, float y);

public:
    int getIterationCounter() const;
    int getAliveCellsCounter() const;
    const std::vector<Cell> &getCells() const;
    Surface();
    void updateB(int numberOfIteration);
    void updateC(int numberOfIteration);
    void clear();

};

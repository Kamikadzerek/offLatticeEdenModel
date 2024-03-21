#include "Cell.h"
#include "CellPrimitive.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <set>
#include <vector>
extern const std::string surfacesPath;
extern const std::string dataPath;
extern const std::string plotsPath;
extern const double WIDTH;
extern const double HEIGHT;
extern const double SIZE;
extern bool GROWINGUP;
extern double PROBABILITY;
extern const int DEADAGE;
extern const double OUTLINETHICNESS;
extern char VERSION;
extern const int NUMBEROFANGLES;
template<typename T>
class Surface// store all cells
{
private:
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
    std::vector<Cell<T>> cells;
    unsigned long prevIndex;
    std::vector<Cell<T> *> prevEdgeCells;
    std::vector<double> angles;
    std::vector<displacement> displacements = {
            {0, SIZE},
            {0, -SIZE},
            {SIZE, 0},
            //            {SIZE, SIZE},
            //            {SIZE, -SIZE},
            {-SIZE, 0},
            //            {-SIZE, SIZE},
            //            {-SIZE, -SIZE},
    };
    double initialX;
    double initialY;
    int iterationCounter;
    int aliveCellsCounter;
    double spawnDistance = SIZE + 2 * OUTLINETHICNESS;
    double cellRadius = spawnDistance / 2;
    std::string name;
    bool cellIsConflicting(const CellPrimitive *cell)
    {
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            for (auto cellTemp = cells.end() - 1; cellTemp != cells.begin(); --cellTemp)
            {
                if (squareIsOverSquare(cellTemp, cell))
                {
                    return true;
                }
            }
            return false;
        }
        else
        {
            for (auto cellTemp = cells.end(); cellTemp > cells.begin() - 1; cellTemp--)
            {
                if (circleIsOverCircle(*cellTemp.base(), *cell))
                {
                    return true;
                }
            }
            return false;
        }
    }
    bool circleIsOverCircle(const Cell<T> &cell1, const CellPrimitive &cell2) const
    {
        if (std::abs(cell1.getX() - cell2.getX()) < 2 * cellRadius)
        {
            if (distanceBtwTwoPoints(cell1.getX(), cell1.getY(), cell2.getX(), cell2.getY()) < 2 * cellRadius)
            {
                return true;
            }
        }
        return false;
    }
    bool squareIsOverSquare(const Cell<T> &cell1, const CellPrimitive &cell2)
    {

        if (std::abs(cell1.getX() - cell2.getX()) < SIZE)
            if (std::abs(cell1.getY() - cell2.getY()) < SIZE)
                return true;
        return false;
    }
    double radiusOfFittedEdge(const sf::CircleShape &edge, const std::vector<Cell<T> *> &edgeCells)
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
                    double d = std::sqrt(std::pow(std::abs(x - cell->getX() + cellRadius), 2) + std::pow(std::abs(y - cell->getY() + cellRadius), 2));// dodatek SIZE/2 wynika z tego że współrzędne (x, y) komórki to współrzędne jej lewego górnego rogu
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
    Surface()
    {
        aliveCellsCounter = 1;
        initialX = WIDTH / 2;
        initialY = HEIGHT / 2;
        iterationCounter = 0;
        cells.reserve(300000);

        if constexpr (std::is_same<T, sf::CircleShape>::value)
        {
            extern const int NUMBEROFANGLES;
            for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / NUMBEROFANGLES)
            {
                angles.push_back(angle);
            }
            name = "Circles" + std::string(1, VERSION);
        }
        else
        {
            name = "Squares" + std::string(1, VERSION);
        }
        cells.emplace_back(initialX, initialY);
    }
    explicit Surface(std::string path)
    {
        std::string line;
        std::ifstream fin;
        fin.open(surfacesPath + "/" + path);
        std::getline(fin, line);
        iterationCounter = std::stoi(line);
        aliveCellsCounter = 0;
        initialX = WIDTH / 2;
        initialY = HEIGHT / 2;
        if constexpr (std::is_same<T, sf::CircleShape>::value)
        {
            extern const int NUMBEROFANGLES;
            for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / NUMBEROFANGLES)
            {
                angles.push_back(angle);
            }
        }
        while (getline(fin, line))
        {
            double x;
            double y;
            bool status;
            std::string buff;
            std::istringstream iss(line);
            int i = 0;
            while (getline(iss, buff, ','))
            {
                if (i == 0)
                {
                    x = stof(buff);
                }
                else if (i == 1)
                {
                    y = stof(buff);
                }
                else if (i == 2)
                {
                    status = buff != "0";
                }
                i++;
            }
            cells.emplace_back(x, y, status);
            if (status) { aliveCellsCounter++; }
        }
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

                std::vector<Cell<sf::CircleShape>::adjacentCell> adjacentCells = cell.getAdjacentCells();
                std::cout << cell << "\n";
                std::cout << "\n--------------------------------------------------------\n";
            }
        }
    }
    sf::Vector2f getCenterOfMassFromLiving()
    {
        double sumX = 0;
        double sumY = 0;
        int counter = 0;
        for (const Cell<T> &cell: cells)
        {
            if (cell.getStatus())
            {
                counter++;
                sumX += cell.getX();
                sumY += cell.getY();
            }
        }
        return sf::Vector2f(sumX / counter, sumY / counter);
    }
    sf::Vector2f getCenterOfMassFromAll()
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
    std::string getName()
    {
        return name;
    }
    sf::CircleShape getEstimateEdge(const std::vector<Cell<T> *> &edgeCells)
    {
        sf::CircleShape edge;
        edge.setPosition(getCenterOfMassFromAll());
        edge.setFillColor(sf::Color(0, 0, 0, 0));
        edge.setOutlineThickness(2);
        edge.setOutlineColor(sf::Color(0, 0, 0, 100));
        //        edge.setRadius(radiusOfFittedEdge(edge, edgeCells));
        edge.setRadius(getMeanRadiusOfLivingCells() / 2 * cellRadius);
        edge.move(-edge.getRadius(), -edge.getRadius());
        return edge;
    }
    int getNumberOfCells()
    {
        return cells.size();
    }
    std::string getNumberOfCellsFormattedString()
    {
        return "NOC" + std::string(7 - std::to_string(cells.size()).size(), '0').append(std::to_string(cells.size()));
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
        std::vector<Cell<sf::CircleShape>::adjacentCell> adjacentCells = {};
        do {
            adjacentCells.clear();
            adjacentCells = currentCell->getAdjacentCells();
            std::sort(adjacentCells.begin(), adjacentCells.end(), [](Cell<sf::CircleShape>::adjacentCell c1, Cell<sf::CircleShape>::adjacentCell c2)
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
    void setAdjacentCells(Cell<T> *cell)
    {
        for (auto cellTemp = cells.begin(); cellTemp != cells.end(); cellTemp++)
        {
            if (cellTemp->getId() != cell->getId())
            {
                if (distanceBtwTwoPoints(cell->getX(), cell->getY(), cellTemp->getX(), cellTemp->getY()) <= sqrt(2 * pow(spawnDistance, 2)))
                {
                    cell->addAdjacentCell(Cell<sf::CircleShape>::adjacentCell(&(*cellTemp), angleBtwTwoPoints(cell->getX(), cell->getY(), cellTemp->getX(), cellTemp->getY())));
                    cellTemp->addAdjacentCell(Cell<sf::CircleShape>::adjacentCell(&(*cell), angleBtwTwoPoints(cellTemp->getX(), cellTemp->getY(), cell->getX(), cell->getY())));
                }
            }
        }
    }
    std::vector<Cell<T> *> getEdgeCellsAltUpdate()
    {
        //        log("\tgetEdgeCellsAltUpdate");
        std::vector<Cell<T> *> edgeCellsBeg(prevEdgeCells.begin(), prevEdgeCells.begin() + prevIndex - 40);
        std::vector<Cell<T> *> edgeCellsEnd(prevEdgeCells.begin() + prevIndex + 40, prevEdgeCells.end());
        std::vector<Cell<T> *> edgeCells;
        edgeCells.insert(edgeCells.end(), edgeCellsBeg.begin(), edgeCellsBeg.end());
        Cell<T> *currentCell = edgeCells.back();
        std::vector<Cell<sf::CircleShape>::adjacentCell> adjacentCells = {};
        do {
            if (currentCell->getId() == edgeCellsEnd.front()->getId())
            {
                break;
            }
            adjacentCells.clear();
            adjacentCells = currentCell->getAdjacentCells();
            std::sort(adjacentCells.begin(), adjacentCells.end(), [](Cell<sf::CircleShape>::adjacentCell c1, Cell<sf::CircleShape>::adjacentCell c2)
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
                if (std::find_if(edgeCells.begin(), edgeCells.end(), [adjacentCell](Cell<T> *cell)
                                 { return adjacentCell->cell->getId() == cell->getId(); }) == edgeCells.end())
                {
                    edgeCells.push_back(adjacentCell->cell);
                    currentCell = edgeCells.back();
                    break;
                }
            }
            //        } while (currentCell->getId() != edgeCellsEnd.front()->getId());
        } while (std::find(edgeCellsEnd.begin(), edgeCellsEnd.end(), currentCell) != edgeCellsEnd.end());
        edgeCells.insert(edgeCells.end(), edgeCellsEnd.begin(), edgeCellsEnd.end());
        return edgeCells;
    }
    int getIterationCounter() const
    {
        return iterationCounter;
    }
    std::string getIterationCounterFormattedString() const
    {
        return "Iter" + std::string(7 - std::to_string(getIterationCounter()).size(), '0').append(std::to_string(getIterationCounter()));
    }
    int getAliveCellsCounter() const
    {
        return aliveCellsCounter;
    }
    const std::vector<Cell<T>> &getCells() const
    {
        return cells;
    }
    const Cell<T> *getFirstCell() const
    {
        return &cells[0];
    }
    void circleUpdateB(int numberOfIteration)
    // In version B, an infection path from all possible paths from infected
    // to adjacent uninfected cells is chosen with the same probability
    // (the original Eden model).
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
                        if (!cellIsConflicting(new const CellPrimitive(x, y)))
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
    // In version C, firstly a alive cell of the cluster is randomly chosen,
    // then an uninfected adjacent cell is randomly chosen to be infected.
    {
        int densityOfMeasurements = 500;
        double lastMeanRadius = 0;
        double currentMeanRadius = 0;
        resetColors();
        for (int i = 0; i < numberOfIteration; i++)
        {
            iterationCounter++;
            unsigned long index = rand() % cells.size();
            auto *cell = &cells[index];
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
                    isConflicting = cellIsConflicting(new const CellPrimitive(x, y));
                    if (!isConflicting)
                    {
                        cells.emplace_back(x, y);
                        cell->addChildCell(&cells.back());
                        setAdjacentCells(&cells.back());
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
            if (iterationCounter % densityOfMeasurements == 0)
            {
                //                saveToFileCenterOfMass("CoM.csv");
                currentMeanRadius = getMeanRadiusOfLivingCells();
                if (currentMeanRadius > lastMeanRadius)
                {
                    currentMeanRadius = lastMeanRadius;
                    //                saveToFileMeanRadiusOfLivingCellsNumOfLivingCells("4dsa.csv");
                    //                saveToFileMaxRadiusSquareOfLivingCellsNumOfLivingCells("DensityLiving.csv");
                    //                saveToFileLivingPerRadius("LivingPerRadius.csv");
                    //                saveToFileLivingPerRadiusTime("LivingPerRadiusTime.csv");
                    //                saveToFileSDMax("5SDLivLivLivMax.csv");
                    //                saveToFileSDMean("5SDLivLivLivMean.csv");
                }
            }
            if (GROWINGUP)
            {
                growingUp();
            }
        }
    }
    void circleUpdateD(int numberOfIteration)
    {
        int densityOfMeasurements = 500;
        double lastMeanRadius = 0;
        double currentMeanRadius = 0;
        resetColors();
        for (int i = 0; i < numberOfIteration; i++)
        {
            iterationCounter++;
            for (Cell<T> &cell: cells)
            {
                if (cell.getStatus() && rand()%1000<PROBABILITY*1000)
                {
                    bool isConflicting;
                    std::shuffle(angles.begin(), angles.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
                    for (const double &angle: angles)
                    {
                        double x = cell.getX() + spawnDistance * cos(angle);
                        double y = cell.getY() + spawnDistance * sin(angle);
                        isConflicting = cellIsConflicting(new const CellPrimitive(x, y));
                        if (!isConflicting)
                        {
                            cells.emplace_back(x, y);
                            cell.addChildCell(&cells.back());
                            setAdjacentCells(&cells.back());
                            aliveCellsCounter++;
                            break;
                        }
                    }
                    if (isConflicting)
                    {
                        cell.death();
                        aliveCellsCounter--;
                    }
                }
            }
            if (iterationCounter % densityOfMeasurements == 0)
            {
                //                saveToFileCenterOfMass("CoM.csv");
                currentMeanRadius = getMeanRadiusOfLivingCells();
                if (currentMeanRadius > lastMeanRadius)
                {
                    currentMeanRadius = lastMeanRadius;
                }
            }
            if (GROWINGUP)
            {
                growingUp();
            }
        }
    }
    void rectangleUpdateA(int numberOfIteration)
    // In version A, a to-be-infected cell is chosen with same probability
    // from all uninfected cells adjacent to the cluster.
    // Version A is version B with deleting duplicate from possibleCoords.
    {
        resetColors();
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
                        if (!cellIsConflicting(new const CellPrimitive(x, y)))
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
        resetColors();
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
                        if (!cellIsConflicting(new const CellPrimitive(x, y)))
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
        resetColors();
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
                if (!cellIsConflicting(new const CellPrimitive(x, y)))
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
            //            saveToFileMeanRadiusOfLivingCells();
        }
    }
    void drawGrowNetwork(sf::RenderWindow *window, sf::RenderTexture *renderTexture, const Cell<T> *cell)
    {
        std::cout << "Start\n";
        //        sf::Vector2f center = getCenterOfMass();
        //        double maxRadius = getMaxRadius();
        //        double cellDistance = distanceBtwTwoPoints(cell->getX(), cell->getY(), center.x, center.y);
        //        int cellColor = 255.0 * cellDistance/maxRadius;
        for (Cell<T> *child: cell->getChildCells())
        {
            //            double childDistance = distanceBtwTwoPoints(cell->getX(), cell->getY(), center.x, center.y);
            //            int childColor = 255.0 * childDistance/maxRadius;
            sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(cell->getX() + cellRadius, cell->getY() + cellRadius)),
                    sf::Vertex(sf::Vector2f(child->getX() + cellRadius, child->getY() + cellRadius))};

            //            line[0].color=sf::Color(255,255, cellColor);
            //            line[1].color = sf::Color(255, 255, childColor);
            line[0].color = sf::Color::Black;
            line[1].color = sf::Color::Black;
            window->draw(line, 2, sf::Lines);
            renderTexture->draw(line, 2, sf::Lines);
            drawGrowNetwork(window, renderTexture, child);
        }
        std::cout << "End\n";
    }
    double getMaxRadiusFromLiving()
    {
        sf::Vector2f center = getCenterOfMassFromLiving();
        double radius = 0;
        double current = 0;
        for (const Cell<T> cell: cells)
        {
            if (cell.getStatus())
            {
                current = distanceBtwTwoPoints(cell.getX(), cell.getY(), center.x, center.y);
                if (current > radius)
                {
                    radius = current;
                }
            }
        }
        return radius;
    }
    double getMaxRadiusFromAll()
    {
        sf::Vector2f center = getCenterOfMassFromAll();
        double radius = 0;
        double current = 0;
        for (const Cell<T> cell: cells)
        {
            if (cell.getStatus())
            {
                current = distanceBtwTwoPoints(cell.getX(), cell.getY(), center.x, center.y);
                if (current > radius)
                {
                    radius = current;
                }
            }
        }
        return radius;
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
    void saveToFile()
    {
        std::ofstream fout;
        std::string fileName;
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            fileName = getName() + getNumberOfCellsFormattedString() + getIterationCounterFormattedString() + ".csv";
        }
        else
        {
            fileName = getName() + getNumberOfCellsFormattedString() + "NOA" + std::to_string(NUMBEROFANGLES) + getIterationCounterFormattedString() + ".csv";
        }
        std::string fullPath = surfacesPath + (surfacesPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath);
        fout << getIterationCounter() << std::endl;//saving to file iteratorCounter
        for (Cell<T> cell: cells)
        {
            fout << cell.getX() << "," << cell.getY() << "," << cell.getStatus() << std::endl;
        }
        fout.close();
        std::cout << "Surface saved to: "
                  << "\"" << fullPath << "\"\n";
    }
    void saveToFile(std::string fileName)
    {
        std::ofstream fout;
        std::string fullPath = surfacesPath + (surfacesPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath);
        fout << getIterationCounter() << std::endl;//saving to file iteratorCounter
        for (Cell<T> cell: cells)
        {
            fout << cell.getX() << "," << cell.getY() << "," << cell.getStatus() << std::endl;
        }
        fout.close();
        std::cout << "Surface saved to: "
                  << "\"" << fullPath << "\"\n";
    }
    double getMeanRadiusOfLivingCells()
    {
        int counter = 0;
        double sum = 0;
        const sf::Vector2f center = getCenterOfMassFromLiving();
        for (const Cell<T> cell: cells)
        {
            if (cell.getStatus())
            {
                counter++;
                sum += distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY());
            }
        }
        return sum / counter;
    }
    double getMeanRadiusOfAllCells()
    {
        int counter = 0;
        double sum = 0;
        const sf::Vector2f center = getCenterOfMassFromAll();
        for (const Cell<T> cell: cells)
        {
            counter++;
            sum += distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY());
        }
        return sum / counter;
    }
    double getSurfaceRoughness(double l)// must be: L%l=0
    {
        std::vector<Cell<T> *> edgeCells = getEdgeCells();
        sf::CircleShape edge = getEstimateEdge(edgeCells);
        double edgeRadius = edge.getRadius();
        sf::Vector2f centerCoords = getCenterOfMassFromLiving();
        std::sort(edgeCells.begin(),
                  edgeCells.end(),
                  [this, centerCoords](Cell<T> *c1, Cell<T> *c2)
                  {
                      return angleBtwTwoPoints(centerCoords.x, centerCoords.y, c1->getX(), c1->getY()) < angleBtwTwoPoints(centerCoords.x, centerCoords.y, c2->getX(), c2->getY());
                  });
        double L = (M_PI * pow(edgeRadius, 2));
        double arc = (l / L) * 2 * M_PI;
        int numberOfSections = L / l;
        std::vector<std::vector<Cell<T> *>> cellSections;
        for (double angle = 0; angle <= 2 * M_PI - arc; angle += arc)
        {
            std::vector<Cell<T> *> tempVector;

            std::copy_if(edgeCells.begin(),
                         edgeCells.end(),
                         std::back_inserter(tempVector),
                         [this, centerCoords, angle, arc](Cell<T> *c)
                         {
                             return angleBtwTwoPoints(centerCoords.x, centerCoords.y, c->getX(), c->getY()) > angle &&
                                    angleBtwTwoPoints(centerCoords.x, centerCoords.y, c->getX(), c->getY()) <= angle + arc;
                         });
            cellSections.push_back(tempVector);
        }
        double sum = 0;
        for (const std::vector<Cell<T> *> section: cellSections)
        {
            double sectionRadius = getEstimateEdge(section).getRadius();
            for (Cell<T> *cell: section)
            {
                sum += pow(distanceBtwTwoPoints(centerCoords.x, centerCoords.y, cell->getX(), cell->getY()) - sectionRadius, 2);
            }
        }
        return sqrt((sum / l) / double(numberOfSections));
    }
    int getNumberOfCellsEnclosedByRadius(double radius)
    {
        int counter = 0;
        sf::Vector2f center = getCenterOfMassFromLiving();
        for (const Cell<T> cell: cells)
        {
            if (distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY()) <= radius)
            {
                counter++;
            }
        }

        return counter;
    }
    void saveToFileAllSurfaceRoughness()
    {
        std::ofstream fout;
        std::string fileName;
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            fileName = "Rgs_" + getName() + getNumberOfCellsFormattedString() + getIterationCounterFormattedString() + ".csv";
        }
        else
        {
            fileName = "Rgs_" + getName() + getNumberOfCellsFormattedString() + "NOA" + std::to_string(NUMBEROFANGLES) + getIterationCounterFormattedString() + ".csv";
        }
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath);
        fout << getIterationCounter() << std::endl;//saving to file iteratorCounter
        double L = M_PI * pow(getEstimateEdge(getEdgeCells()).getRadius(), 2);
        double step = L / 100;
        double end = L / 2;
        std::cout << "Saving data start!\n";
        for (double l = step; l <= end; l += step)
        {
            std::cout << std::fixed << std::setprecision(2) << l / end * 100 << "%"
                      << "\n";
            fout << l << "\t" << getSurfaceRoughness(l) << "\n";
        }
        fout.close();
        std::cout << "Data saved to: "
                  << "\"" << fullPath << "\"\n";
    }
    void saveToFileAllNumberOfCellsEnclosedByRadius()
    {
        std::ofstream fout;
        std::string fileName;
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            fileName = "NumOfCellsEncByRad_" + getName() + getNumberOfCellsFormattedString() + getIterationCounterFormattedString() + ".csv";
        }
        else
        {
            fileName = "NumOfCellsEncByRad_" + getName() + getNumberOfCellsFormattedString() + "NOA" + std::to_string(NUMBEROFANGLES) + getIterationCounterFormattedString() + ".csv";
        }
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath);
        fout << getIterationCounter() << std::endl;//saving to file iteratorCounter
        double R = getEstimateEdge(getEdgeCells()).getRadius() + 100;
        double step = 1;
        std::cout << "Saving data start!\n";
        for (double r = step; r <= R; r += step)
        {
            std::cout << std::fixed << std::setprecision(2) << r / R * 100 << "%"
                      << "\n";
            fout << std::pow(r / cellRadius, 2) << "\t" << getNumberOfCellsEnclosedByRadius(r) << "\n";
        }
        fout.close();
        std::cout << "Data saved to: "
                  << "\"" << fullPath << "\"\n";
    }
    void saveToFileAllNumberOfCellsEnclosedByRadius(std::string fileName)
    {
        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath);
        fout << getIterationCounter() << std::endl;//saving to file iteratorCounter
        double R = getEstimateEdge(getEdgeCells()).getRadius() + 400;
        double step = 5;
        std::cout << "Saving data start!\n";
        for (double r = step; r <= R; r += step)
        {
            //            std::cout << std::fixed << std::setprecision(2) << r / R * 100 << "%"
            //                      << "\n";
            fout << std::pow(r / cellRadius, 2) << "\t" << getNumberOfCellsEnclosedByRadius(r) << "\n";
        }
        fout.close();
        std::cout << "Data saved to: "
                  << "\"" << fullPath << "\"\n";
    }
    void saveToFileMeanRadiusOfLivingCellsNumOfLivingCells(std::string fileName)
    {

        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << 2 * getMeanRadiusOfLivingCells() / cellRadius << "\t" << aliveCellsCounter << "\n";// poprawka błąd w pracy
        fout.close();
    }
    void saveToFileMaxRadiusSquareOfLivingCellsNumOfLivingCells(std::string fileName)
    {

        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << pow(getMaxRadiusFromAll(), 2) << "\t" << aliveCellsCounter << "\n";
        fout.close();
    }
    void saveToFileLivingPerRadius(std::string fileName)
    {
        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << getMeanRadiusOfLivingCells() << "\t" << aliveCellsCounter / getMeanRadiusOfLivingCells() << "\n";
        fout.close();
    }
    void saveToFileLivingPerRadiusTime(std::string fileName)
    {
        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << iterationCounter << "\t" << aliveCellsCounter / getMeanRadiusOfLivingCells() << "\n";
        fout.close();
    }
    void saveToFileCenterOfMass(std::string fileName)
    {
        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        auto center = getCenterOfMassFromAll();
        fout << center.x << "\t" << center.y << "\n";
        fout.close();
    }
    double getSD()
    {
        double meanRadius = getMeanRadiusOfLivingCells();
        const sf::Vector2f center = getCenterOfMassFromLiving();
        std::vector<double> deviations;
        for (const Cell<T> cell: cells)
        {
            if (cell.getStatus())
            {
                deviations.push_back(pow(meanRadius - distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY()), 2));
            }
        }
        double variance = 0;
        for (double v: deviations)
        {
            variance += v;
        }
        variance /= deviations.size();
        return sqrt(variance) / cellRadius;
    }
    void saveToFileSDMax(std::string fileName)
    {
        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << getMaxRadiusFromAll() / cellRadius << "\t" << getSD() << "\n";
        fout.close();
    }
    void saveToFileSDMean(std::string fileName)
    {
        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << getMeanRadiusOfLivingCells() / cellRadius << "\t" << getSD() << "\n";
        fout.close();
    }
    void saveToFileTimeOfRunning(std::string fileName, double duration)
    {

        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << duration << "\t" << cells.size() << "\n";
        fout.close();
    }

    static void log(std::string string)
    {

        std::ofstream fout;
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "../log";
        fout.open(fullPath, std::ios_base::app);
        fout << std::chrono::system_clock::now() << ";\t" << string << "\n";
        fout.close();
    }
    void growingUp()
    {
        for (Cell<T> &cell: cells)
        {
            cell.growingUp();
            if (cell.getStatus() && cell.getAge() >= DEADAGE)
            {
                cell.death();
                aliveCellsCounter--;
            }
        }
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
//mierzenien środka od wszystkich albo tylko żywych
// grow network
// wyznacznie promienia
// co powoduje fluktuacje w SD
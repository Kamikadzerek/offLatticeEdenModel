#include "BS_thread_pool.hpp"
#include "Cell.h"
#include "CellPrimitive.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "gnuplot-iostream.h"
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <set>
#include <thread>
#include <vector>
extern const std::string surfacesPath;
extern const std::string dataPath;
extern const std::string plotsPath;
extern const double WIDTH;
extern const double HEIGHT;
extern const double SIZE;
extern char VERSION;
extern const int NUMBEROFANGLES;
template<typename T>
class Surface// store all cells
{
private:
    struct adjacentCell
    {
        Cell<T> *cell;
        double angle;
    };
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
    double spawnDistance = SIZE;//*1.0005;
    std::string name;
    bool cellIsConflicting(const CellPrimitive cell)
    {
        BS::thread_pool pool;
        std::vector<std::future<bool>> my_futures;
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
                //                if(circleIsOverCircle(cellTemp.base(), &cell)){
                //                    return true;
                //                }
                my_futures.push_back(pool.submit_task(
                        [this, cell, cellTemp]
                        {
                            return circleIsOverCircle(cellTemp.base(), &cell);
                        }));
            }
            for (auto &future: my_futures)
            {
                if (future.valid())
                    if (future.wait_for(std::chrono::microseconds(200)) == std::future_status::ready)
                        if (future.get())
                            return true;
            }
            return false;
        }
    }
    bool circleIsOverCircle(const Cell<T> *cell1, const CellPrimitive *cell2) const
    {
        double radius = SIZE / 2;
        if (std::abs(cell1->getX() - cell2->getX()) < 2 * radius)
        {
            if (distanceBtwTwoPoints(cell1->getX(), cell1->getY(), cell2->getX(), cell2->getY()) < 2 * radius)
            {
                return true;
            }
        }
        return false;
    }
    bool squareIsOverSquare(const Cell<T> &cell1, const CellPrimitive *cell2)
    {

        if (std::abs(cell1->getX() - cell2->getX()) < SIZE)
            if (std::abs(cell1->getY() - cell2->getY()) < SIZE)
                return true;
        return false;
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
        fin.open(surfacesPath + path);
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
    std::vector<adjacentCell> getAdjacentCells(const Cell<T> *cell)
    {
        std::vector<Surface::adjacentCell> adjacentCells = {};
        for (auto cellTemp = cells.begin(); cellTemp != cells.end(); cellTemp++)
        {
            if (cellTemp->getId() != cell->getId())
            {
                if (distanceBtwTwoPoints(cell->getX(), cell->getY(), cellTemp->getX(), cellTemp->getY()) <= sqrt(2 * pow(spawnDistance, 2)))
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
                //                                std::cout << adjacentCells;
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
    std::string getName()
    {
        return name;
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
        std::vector<Surface::adjacentCell> adjacentCells = {};
        //        bool isDone = false;
        do {
            adjacentCells.clear();
            adjacentCells = getAdjacentCells(currentCell);
            std::sort(adjacentCells.begin(), adjacentCells.end(), [](Surface::adjacentCell c1, Surface::adjacentCell c2)
                      { return c1.angle < c2.angle; });
            for (auto adjacentCell = adjacentCells.begin(); adjacentCell < adjacentCells.end(); adjacentCell++)
            {
                //                isDone = false;
                //                if (!isDone)
                //                {
                if (adjacentCells.size() == 1)
                {
                    edgeCells.push_back(adjacentCells.back().cell);
                    currentCell = edgeCells.back();
                    //                    isDone = true;
                    break;
                }
                if (edgeCells.size() < 2)
                {
                    edgeCells.push_back(adjacentCells.begin()->cell);
                    currentCell = edgeCells.back();
                    //                    isDone = true;
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
                    //                    isDone = true;
                    break;
                }
                //                }
            }

        } while (currentCell != farRightCell);
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
                        if (!cellIsConflicting(CellPrimitive(x, y)))
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
            saveToFileMeanRadiusOfLivingCells();
        }
    }
    void circleUpdateC(int numberOfIteration)
    // In version C, firstly a boundary cell of the cluster is randomly chosen,
    // then an uninfected adjacent cell is randomly chosen to be infected.
    {
        //        int lastAliveCellsCounter = 0;
        //        int currentAliveCellsCounter = 0;
        //        double lastMeanRadius = 0;
        //        double currentMeanRadius = 0;
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
                    isConflicting = cellIsConflicting(CellPrimitive(x, y));
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
            //for each NumberOfCell
            //            currentAliveCellsCounter = getAliveCellsCounter();
            //            if(currentAliveCellsCounter>lastAliveCellsCounter){
            //                saveToFileMeanRadiusOfLivingCells();
            //                lastAliveCellsCounter = currentAliveCellsCounter;
            //            }
            //for each Radius
            //            currentMeanRadius = getMeanRadiusOfLivingCells();
            //            if(currentMeanRadius>lastMeanRadius){
            //                saveToFileMeanRadiusOfLivingCells();
            //                lastMeanRadius = currentMeanRadius;
            //            }
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
                        if (!cellIsConflicting(CellPrimitive(x, y)))
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
            saveToFileMeanRadiusOfLivingCells();
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
                        if (!cellIsConflicting(CellPrimitive(x, y)))
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
            saveToFileMeanRadiusOfLivingCells();
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
                if (!cellIsConflicting(CellPrimitive(x, y)))
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
    double getMeanRadiusOfLivingCells()
    {
        int counter = 0;
        double sum = 0;

        for (const Cell<T> cell: cells)
        {
            if (cell.getStatus())
            {
                sf::Vector2f center = getCenterOfMass();
                counter++;
                sum += distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY());
            }
        }
        return sum / counter;
    }
    double getSurfaceRoughness(double l)// must be: L%l=0
    {
        std::vector<Cell<T> *> edgeCells = getEdgeCells();
        sf::CircleShape edge = getEstimateEdge(edgeCells);
        double edgeRadius = edge.getRadius();
        sf::Vector2f centerCoords = getCenterOfMass();
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
        sf::Vector2f center = getCenterOfMass();
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
            fout << r << "\t" << getNumberOfCellsEnclosedByRadius(r) << "\n";
        }
        fout.close();
        std::cout << "Data saved to: "
                  << "\"" << fullPath << "\"\n";
    }
    void saveToFileMeanRadiusOfLivingCells()
    {

        std::ofstream fout;
        std::string fileName;
        if constexpr (std::is_same<T, sf::RectangleShape>::value)
        {
            fileName = "MeanRadOfLivCells_S" + std::string(1, VERSION) + ".csv";
        }
        else
        {
            fileName = "MeanRadOfLivCells_C" + std::string(1, VERSION) + "NOA" + std::to_string(NUMBEROFANGLES) + ".csv";
        }
        std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + fileName;
        fout.open(fullPath, std::ios_base::app);
        fout << getMeanRadiusOfLivingCells() << "\t" << getAliveCellsCounter() << "\n";
        fout.close();
    }
    void generateAndSaveToFile() {}
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

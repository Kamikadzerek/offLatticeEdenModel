#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <vector>
#include "Cell.h"
#include "CellPrimitive.h"
#include "octree.h"
extern const std::string dataPath;
extern const double SIZE;
class Surface// store all cells
{
  private:
  Octree<std::vector<Cell *>> tree = Octree<std::vector<Cell *>>(4096);
  int densityOfMeasurements = 100;
  double lastMeanRadius = 0;
  double currentMeanRadius = 0;
  struct angle {
    double sin;
    double cos;
  };
  struct vector2d {
    double x;
    double y;
  };
  std::vector<Cell> cells;
  std::vector<Cell *> aliveCells;
  std::vector<angle> angles;
  double initialX = 0;
  double initialY = 0;
  double initialZ = 0;
  int id;
  int iterationCounter = 0;
  int aliveCellsCounter;
  double spawnDistance = SIZE;
  double cellRadius = spawnDistance / 2;
  std::random_device rd;
  std::mt19937 gen;
  bool circleIsOverCircle(const Cell &cell1, const Cell cell2) const {
    if (std::abs(cell1.getX() - cell2.getX()) < 2 * cellRadius) {
      if (std::abs(cell1.getY() - cell2.getY()) < 2 * cellRadius) {
        if (dstBtw2Pts(cell1, cell2) < 2 * cellRadius) {
          return true;
        }
      }
    }
    return false;
  }

  public:
  Surface(int i) : id(i) {
    extern const double LIMITOFCELLS;
    extern const int NUMBEROFANGLES;
    gen = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
    aliveCellsCounter = 1;
    cells.reserve(LIMITOFCELLS + 200);
    for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / NUMBEROFANGLES) {
      angles.emplace_back(sin(angle), cos(angle));
    }
    addCell(initialX, initialY, initialZ);
  }
  ~Surface() {
    cells.clear();
  }
  void addCell(double x, double y, double z) {
    cells.emplace_back(x, y, z);
    aliveCells.emplace_back(&cells.back());
    aliveCellsCounter++;
    if (tree(x + 2048, y + 2048, z + 2048) != tree.emptyValue()) {
      tree(x + 2048, y + 2048, z + 2048).push_back(&cells.back());
    } else {
      tree(x + 2048, y + 2048, z + 2048) = std::vector<Cell *>();
      tree(x + 2048, y + 2048, z + 2048).push_back(&cells.back());
    }
  }
  void deadCell(Cell *c) {
    c->death();
    auto it = std::find(aliveCells.begin(), aliveCells.end(), c);
    if (it != aliveCells.end()) {
      std::swap(*it, aliveCells.back());
      aliveCells.pop_back();
    } else {
      std::cout << "No element in vector\n";
    }
    aliveCellsCounter--;
  }
  bool cellIsConflicting(const Cell cell) {
    int x = int(cell.getX()) + 2048;
    int y = int(cell.getY()) + 2048;
    int z = int(cell.getZ()) + 2048;
    for (int i = x - 1; i <= x + 1; i++) {
      for (int j = y - 1; j <= y + 1; j++) {
        for (int k = z - 1; k <= z + 1; k++) {
          if (!tree(i, j, k).empty()) {
            for (auto cellTemp : tree(i, j, k)) {
              if (circleIsOverCircle(*cellTemp, cell)) {
                return true;
              }
            }
          }
        }
      }
    }
    return false;
  }
  vector2d getCenterOfMassFromLiving() {
    double sumX = 0;
    double sumY = 0;
    int counter = 0;
    for (const Cell *cell : aliveCells) {
      counter++;
      sumX += cell->getX();
      sumY += cell->getY();
    }
    return vector2d(sumX / counter, sumY / counter);
  }
  int getNumberOfCells() { return cells.size(); }
  int getAliveCellsCounter() const { return aliveCellsCounter; }
  void update(int numberOfIteration) {
    for (int i = 0; i < numberOfIteration; i++) {
      iterationCounter++;
      unsigned long index = gen() % aliveCells.size();
      Cell *cell = aliveCells[index];
      if (cell->getStatus()) {
        bool isConflicting;
        std::shuffle(angles.begin(), angles.end(), gen);
        auto theta = angles;
        std::shuffle(angles.begin(), angles.end(), gen);
        auto phi = angles;
        for (const angle &angle1 : theta) {
          for (const angle &angle2 : phi) {
            double z = cell->getZ() + spawnDistance * angle1.sin;
            isConflicting = z > SIZE * 5 || z < -SIZE * 5;
            if (!isConflicting) {
              double x = cell->getX() + spawnDistance * angle1.cos * angle2.cos;
              double y = cell->getY() + spawnDistance * angle1.cos * angle2.sin;
              isConflicting = cellIsConflicting(Cell(x, y, z));
              if (!isConflicting) {
                addCell(x, y, z);
                break;
              }
            }
          }
        }
        if (isConflicting) {
          deadCell(cell);
        }
      }
      if (iterationCounter % densityOfMeasurements == 0) {
        currentMeanRadius = getMeanRadiusOfLivingCells();
        if (currentMeanRadius > lastMeanRadius + 2) {
          lastMeanRadius = currentMeanRadius;
          saveToFileMeanRadiusOfLivingCellsNumOfLivingCells("Id" + std::to_string(id) + ".csv");
          saveToFileSDMean("Id" + std::to_string(id) + ".csv");
        }
      }
    }
  }
  static double dstBtw2Pts(const Cell c1, const Cell c2) {
    return sqrt(pow(std::abs(c1.getX() - c2.getX()), 2) + pow(std::abs(c1.getY() - c2.getY()), 2) + pow(std::abs(c1.getZ() - c2.getZ()), 2));
  }
  static double dstBtw2Pts(const vector2d v, const Cell c) {
    return sqrt(pow(std::abs(v.x - c.getX()), 2) + pow(std::abs(v.y - c.getY()), 2));
  }
  static double dstBtw2Pts(const vector2d v, const Cell *c) {
    return sqrt(pow(std::abs(v.x - c->getX()), 2) + pow(std::abs(v.y - c->getY()), 2));
  }
  double getMeanRadiusOfLivingCells() {
    int counter = 0;
    double sum = 0;
    const vector2d center = getCenterOfMassFromLiving();
    for (const Cell *cell : aliveCells) {
      counter++;
      sum += dstBtw2Pts(center, cell);
    }
    return sum / counter;
  }
  int getNumberOfCellsEnclosedByRadius(double radius) {
    int counter = 0;
    vector2d center = getCenterOfMassFromLiving();
    for (const Cell cell : cells) {
      if (dstBtw2Pts(center, cell) <= radius) {
        counter++;
      }
    }
    return counter;
  }
  void saveToFileAllNumberOfCellsEnclosedByRadius(std::string fileName) {
    std::ofstream fout;
    std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "3D/Fig3/" + fileName;
    fout.open(fullPath);
    double R = getMeanRadiusOfLivingCells() + 400;
    double step = 5;
    for (double r = step; r <= R; r += step) {
      fout << std::pow(r / cellRadius, 2) << "\t" << getNumberOfCellsEnclosedByRadius(r) << "\n";
    }
    fout.close();
  }
  void saveToFileMeanRadiusOfLivingCellsNumOfLivingCells(std::string fileName) {
    std::ofstream fout;
    std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "3D/Fig4/" + fileName;
    fout.open(fullPath, std::ios_base::app);
    fout << 2 * getMeanRadiusOfLivingCells() / cellRadius << "\t" << aliveCellsCounter << "\n";// poprawka błąd w pracy
    fout.close();
  }
  double getSD() {
    double meanRadius = getMeanRadiusOfLivingCells();
    const vector2d center = getCenterOfMassFromLiving();
    std::vector<double> deviations;
    for (const Cell *cell : aliveCells) {
      deviations.push_back(pow(meanRadius - dstBtw2Pts(center, cell), 2));
    }
    double variance = 0;
    for (double v : deviations) {
      variance += v;
    }
    variance /= deviations.size();
    return sqrt(variance) / cellRadius;
  }
  void saveToFileSDMean(std::string fileName) {
    std::ofstream fout;
    std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "3D/Fig5/" + fileName;
    fout.open(fullPath, std::ios_base::app);
    fout << getMeanRadiusOfLivingCells() / cellRadius << "\t" << getSD() << "\n";
    fout.close();
  }
  void saveToFile(std::string fileName) {
    std::ofstream fout;
    std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "3D/Spaces/" + fileName;
    fout.open(fullPath);
    for (auto const cell : cells) {
      fout << cell.getX() * 10 << "\t" << cell.getY() * 10 << "\t" << cell.getZ() * 10 << "\t";
      if (cell.getStatus()) {
        fout << "1";
      } else {
        fout << "0";
      }
      fout << "\n";
    }
    fout.close();
  }
};
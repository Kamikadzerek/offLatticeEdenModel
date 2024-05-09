#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <vector>
#include "Cell.h"
#include "octree.h"
extern const double MAXRADIUS;
class Surface// store all cells
{
  private:
  std::vector<std::vector<int>> conflictQueue = {
      std::vector<int>({0, 0, 0}),
      std::vector<int>({1, 0, 0}),
      std::vector<int>({-1, 0, 0}),
      std::vector<int>({0, 1, 0}),
      std::vector<int>({0, -1, 0}),
      std::vector<int>({0, 0, 1}),
      std::vector<int>({0, 0, -1}),
      std::vector<int>({1, 1, 0}),
      std::vector<int>({1, -1, 0}),
      std::vector<int>({1, 0, 1}),
      std::vector<int>({1, 0, -1}),
      std::vector<int>({-1, 1, 0}),
      std::vector<int>({-1, -1, 0}),
      std::vector<int>({-1, 0, 1}),
      std::vector<int>({-1, 0, -1}),
      std::vector<int>({0, 1, 1}),
      std::vector<int>({0, 1, -1}),
      std::vector<int>({0, -1, 1}),
      std::vector<int>({0, -1, -1}),
      std::vector<int>({1, 1, 1}),
      std::vector<int>({1, 1, -1}),
      std::vector<int>({1, -1, 1}),
      std::vector<int>({1, -1, -1}),
      std::vector<int>({-1, 1, 1}),
      std::vector<int>({-1, 1, -1}),
      std::vector<int>({-1, -1, 1}),
      std::vector<int>({-1, -1, -1})};
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
  int numOfAngles;
  int stateCounter = 0;
  int iterationCounter = 0;
  int aliveCellsCounter;
  double thickness;
  double spawnDistance = 1;
  double cellRadius = spawnDistance / 2;
  std::random_device rd;
  std::mt19937 gen;
  [[nodiscard]] bool circleIsOverCircle(const Cell &cell1, const Cell cell2) const {
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
  explicit Surface(int i, int numberOfAngles, double thick) : id(i), numOfAngles(numberOfAngles), thickness(thick) {
    extern const double LIMITOFCELLS;
    extern const int ITERATIONBYONE;
    gen = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
    aliveCellsCounter = 1;
    cells.reserve(pow(MAXRADIUS / cellRadius, 2) * (thickness + 1) + 2 * ITERATIONBYONE);
    for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / numOfAngles) {
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
    if (tree(std::floor(x) + 2048, std::floor(y) + 2048, std::floor(z) + 2048) != tree.emptyValue()) {
      tree(std::floor(x) + 2048, std::floor(y) + 2048, std::floor(z) + 2048).push_back(&cells.back());
    } else {
      tree(std::floor(x) + 2048, std::floor(y) + 2048, std::floor(z) + 2048) = std::vector<Cell *>();
      tree(std::floor(x) + 2048, std::floor(y) + 2048, std::floor(z) + 2048).push_back(&cells.back());
    }
  }
  void deadCell(Cell *c) {
    c->death();
    auto it = std::find(aliveCells.begin(), aliveCells.end(), c);
    if (it != aliveCells.end()) {
      std::swap(*it, aliveCells.back());
      aliveCells.pop_back();
    }
    aliveCellsCounter--;
  }
  bool cellIsConflicting(const Cell cell) {
    int x = std::floor(cell.getX()) + 2048;
    int y = std::floor(cell.getY()) + 2048;
    int z = std::floor(cell.getZ()) + 2048;
    for (auto dir : conflictQueue) {
      for (auto const cellTemp : tree(x + dir[0], y + dir[1], z + dir[2])) {
        if (circleIsOverCircle(*cellTemp, cell)) {
          return true;
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
  [[maybe_unused]] [[nodiscard]] int getAliveCellsCounter() const { return aliveCellsCounter; }
  void update(int numberOfIteration) {
    for (int i = 0; i < numberOfIteration; i++) {
      iterationCounter++;
      unsigned long index = gen() % aliveCells.size();
      Cell *cell = aliveCells[index];
      if (cell->getStatus()) {
        bool isConflicting;
        if (thickness > 1) {
          // 3D VERSION -------------------------------------------------
          std::shuffle(angles.begin(), angles.end(), gen);
          auto theta = angles;
          std::shuffle(angles.begin(), angles.end(), gen);
          auto phi = angles;
          for (const angle &angle1 : theta) {
            for (const angle &angle2 : phi) {
              double z = cell->getZ() + angle1.sin;
              isConflicting = z > thickness / 2 || z < -thickness / 2;
              if (!isConflicting) {
                double x = cell->getX() + angle1.cos * angle2.cos;
                double y = cell->getY() + angle1.cos * angle2.sin;
                isConflicting = cellIsConflicting(Cell(x, y, z));
                if (!isConflicting) {
                  addCell(x, y, z);
                  break;
                }
              }
            }
          }
        } else {
          // 2D VERSION -------------------------------------------------
          std::shuffle(angles.begin(), angles.end(), gen);
          for (const angle &angle : angles) {
            double x = cell->getX() + angle.cos;
            double y = cell->getY() + angle.sin;
            double z = 0;
            isConflicting = cellIsConflicting(Cell(x, y, z));
            if (!isConflicting) {
              addCell(x, y, z);
              break;
            }
          }
        }
        // COMMON PART -------------------------------------------------
        if (isConflicting) {
          deadCell(cell);
        }
      }
      if (iterationCounter % densityOfMeasurements == 0) {
        currentMeanRadius = getMeanRadiusOfLivingCells();
        if (currentMeanRadius > lastMeanRadius + 2) {
          lastMeanRadius = currentMeanRadius;
          //          saveToFileMeanRadiusOfLivingCellsNumOfLivingCells("Id" + std::to_string(id) + ".csv");
          //          saveToFileSDMean("Id" + std::to_string(id) + ".csv");
          //          saveToFile("Id" + std::to_string(id) + "_" + std::to_string(stateCounter++) + ".csv");
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
  [[maybe_unused]] void saveToFileAllNumberOfCellsEnclosedByRadius(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = std::to_string(thickness) + "Thickness/Density/" + fileName;
    fout.open(fullPath);
    double R = getMeanRadiusOfLivingCells() + 100;
    double step = 5;
    for (double r = step; r <= R; r += step) {
      fout << std::pow(r / cellRadius, 2) << "\t" << getNumberOfCellsEnclosedByRadius(r) << "\n";
    }
    fout.close();
  }
  [[maybe_unused]] void saveToFileMeanRadiusOfLivingCellsNumOfLivingCells(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = std::to_string(thickness) + "Thickness/Growth/" + fileName;
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
  void saveToFileSDMean(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = std::to_string(thickness) + "Thickness/SD/" + fileName;
    fout.open(fullPath, std::ios_base::app);
    fout << getMeanRadiusOfLivingCells() / cellRadius << "\t" << getSD() << "\n";
    fout.close();
  }
  [[maybe_unused]] void saveToFile(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = std::to_string(thickness) + "Thickness/Spaces/" + fileName;
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

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <vector>
#include "Cell.h"
#include "CellPrimitive.h"
extern const std::string dataPath;
extern const double SIZE;
class Surface// store all cells
{
  private:
  struct vector2d {
    double x;
    double y;
  };
  std::vector<Cell> cells;
  std::vector<double> angles;
  double initialX;
  double initialY;
  int id;
  int iterationCounter;
  int aliveCellsCounter;
  double spawnDistance = SIZE;
  double cellRadius = spawnDistance / 2;
  std::random_device rd;
  std::mt19937 gen;
  bool circleIsOverCircle(const Cell &cell1, const CellPrimitive &cell2) const {
    if (std::abs(cell1.getX() - cell2.getX()) < 2 * cellRadius) {
      if (distanceBtwTwoPoints(cell1.getX(), cell1.getY(), cell2.getX(), cell2.getY()) < 2 * cellRadius) {
        return true;
      }
    }
    return false;
  }

  public:
  void addCell(double x, double y) {
    cells.emplace_back(x, y);
    aliveCellsCounter++;
  }
  bool cellIsConflicting(const CellPrimitive *cell) {
    for (auto cellTemp = cells.end(); cellTemp > cells.begin() - 1; cellTemp--) {
      if (circleIsOverCircle(*cellTemp.base(), *cell)) {
        return true;
      }
    }
    return false;
  }
  Surface(int i) : id(i) {
    gen = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
    aliveCellsCounter = 1;
    initialX = 0;
    initialY = 0;
    iterationCounter = 0;
    cells.reserve(300000);
    extern const int NUMBEROFANGLES;
    for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / NUMBEROFANGLES) {
      angles.push_back(angle);
    }
    cells.emplace_back(initialX, initialY);
  }
  ~Surface() {
      cells.clear();

  }
  vector2d getCenterOfMassFromLiving() {
    double sumX = 0;
    double sumY = 0;
    int counter = 0;
    for (const Cell &cell : cells) {
      if (cell.getStatus()) {
        counter++;
        sumX += cell.getX();
        sumY += cell.getY();
      }
    }
    return vector2d(sumX / counter, sumY / counter);
  }
  int getNumberOfCells() { return cells.size(); }
  int getIterationCounter() const { return iterationCounter; }
  int getAliveCellsCounter() const { return aliveCellsCounter; }
  const std::vector<Cell> &getCells() const { return cells; }
  void update(int numberOfIteration) {
    int densityOfMeasurements = 100;
    double lastMeanRadius = 0;
    double currentMeanRadius = 0;
    for (int i = 0; i < numberOfIteration; i++) {
      iterationCounter++;
      unsigned long index = gen() % cells.size();
      auto *cell = &cells[index];
      while (!cell->getStatus()) {
        index = gen() % cells.size();
        cell = &cells[index];
      }
      if (cell->getStatus()) {
        bool isConflicting;
        std::shuffle(angles.begin(), angles.end(), std::mt19937(std::chrono::system_clock::now().time_since_epoch().count()));
        for (const double &angle : angles) {
          double x = cell->getX() + spawnDistance * cos(angle);
          double y = cell->getY() + spawnDistance * sin(angle);
          isConflicting = cellIsConflicting(new const CellPrimitive(x, y));
          if (!isConflicting) {
            addCell(x,y);
            break;
          }
        }
        if (isConflicting) {
          cell->death();
          aliveCellsCounter--;
        }
      }
//            if (iterationCounter % densityOfMeasurements == 0) {
//              currentMeanRadius = getMeanRadiusOfLivingCells();
//              if (currentMeanRadius > lastMeanRadius+2) {
//                lastMeanRadius = currentMeanRadius;
//                saveToFileMeanRadiusOfLivingCellsNumOfLivingCells("Id"+std::to_string(id)+".csv");
//                saveToFileSDMean("Id"+std::to_string(id)+".csv");
//              }
//            }
    }
  }
  void clear() {
    Cell::resetCounter();
    aliveCellsCounter = 0;
    iterationCounter = 0;
    cells.clear();
    cells.emplace_back(initialX, initialY);
  }
  static double distanceBtwTwoPoints(double x1, double y1, double x2, double y2) {
    return sqrt(pow(std::abs(x1 - x2), 2) + pow(std::abs(y1 - y2), 2));
  }
  double getMeanRadiusOfLivingCells() {
    int counter = 0;
    double sum = 0;
    const vector2d center = getCenterOfMassFromLiving();
    for (const Cell cell : cells) {
      if (cell.getStatus()) {
        counter++;
        sum += distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY());
      }
    }
    return sum / counter;
  }
  int getNumberOfCellsEnclosedByRadius(double radius) {
    int counter = 0;
    vector2d center = getCenterOfMassFromLiving();
    for (const Cell cell : cells) {
      if (distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY()) <= radius) {
        counter++;
      }
    }
    return counter;
  }
  void saveToFileAllNumberOfCellsEnclosedByRadius(std::string fileName) {
    std::ofstream fout;
    std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "Fig3/" + fileName;
    fout.open(fullPath);
    double R = getMeanRadiusOfLivingCells() + 400;
    double step = 5;
    //    std::cout << "Saving data start!\n";
    for (double r = step; r <= R; r += step) {
      fout << std::pow(r / cellRadius, 2) << "\t" << getNumberOfCellsEnclosedByRadius(r) << "\n";
    }
    fout.close();
    //    std::cout << "Data saved to: "
    //              << "\"" << fullPath << "\"\n";
  }
  void saveToFileMeanRadiusOfLivingCellsNumOfLivingCells(std::string fileName) {
    std::ofstream fout;
    std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "Fig4/" + fileName;
    fout.open(fullPath, std::ios_base::app);
    fout << 2 * getMeanRadiusOfLivingCells() / cellRadius << "\t" << aliveCellsCounter << "\n";// poprawka błąd w pracy
    fout.close();
  }
  double getSD() {
    double meanRadius = getMeanRadiusOfLivingCells();
    const vector2d center = getCenterOfMassFromLiving();
    std::vector<double> deviations;
    for (const Cell cell : cells) {
      if (cell.getStatus()) {
        deviations.push_back(pow(meanRadius - distanceBtwTwoPoints(center.x, center.y, cell.getX(), cell.getY()), 2));
      }
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
    std::string fullPath = dataPath + (dataPath.back() != '/' ? "/" : "") + "Fig5/" + fileName;
    fout.open(fullPath, std::ios_base::app);
    fout << getMeanRadiusOfLivingCells() / cellRadius << "\t" << getSD() << "\n";       // !!!!!!!!!!!!!!!!!!! NA CHUJ MNOŻYSZ RAZY DWA??? !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    fout.close();
  }
};
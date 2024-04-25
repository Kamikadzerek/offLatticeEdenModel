#include <filesystem>
#include <iostream>
#include <random>
#include "Surface.h"
extern const std::string dataPath = std::filesystem::path(std::filesystem::current_path()) /= "../../../dane/MT";
extern const int NUMBEROFANGLES = 180;
int LIMITOFCELLS = 2000;
int ITERATIONBYONE = 10000;
//----------------------------------------------------------------
bool DRAWONLYALIVE = false;
bool DRAWCELLS = true;
bool DRAWEDGE = true;
extern const double SIZE = 1;
auto start = std::chrono::high_resolution_clock::now();
auto stop = std::chrono::high_resolution_clock::now();
int main(int argc, char *argv[]) {
  //  for (int i = 81; i < 100; i++) {
  Surface surface(1);
  while (surface.getAliveCellsCounter() <= LIMITOFCELLS) {
    surface.update(ITERATIONBYONE);
  }
  //    surface.saveToFileAllNumberOfCellsEnclosedByRadius("Id" + std::to_string(i) + ".csv");
  //    std::cout << i << "\n";
  //  }
  return 0;
}

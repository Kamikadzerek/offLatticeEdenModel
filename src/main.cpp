#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include "Surface.h"
extern const std::string dataPath = std::filesystem::path(std::filesystem::current_path()) /= "../../../dane/MT";
extern const int NUMBEROFANGLES = 180;
double LIMITOFCELLS = 200000;
int ITERATIONBYONE = 10000;
//----------------------------------------------------------------
extern const double SIZE = 1;
int main(int argc, char *argv[]) {
  //  for (int i = atoi(argv[1]); i < atoi(argv[2]); i++) {
  for (int i = 0; i < 10; i++) {
    Surface surface(i);
    double numberOfCells = surface.getNumberOfCells();
    while (numberOfCells <= LIMITOFCELLS) {
      std::cout << i << ": " << numberOfCells / LIMITOFCELLS * 100 << "%\n";
      surface.update(ITERATIONBYONE);
      numberOfCells = surface.getNumberOfCells();
      std::cout << "\033[A\r\33[2K";
    }
    //          surface.saveToFileAllNumberOfCellsEnclosedByRadius("Id" + std::to_string(i) + ".csv");
  }
  return 0;
}
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include "Surface.h"
const double NUMBEROFANGLES = 360;
extern const double LIMITOFCELLS = 200000;
// extern const double MAXRADIUS = 275;
extern const double MAXRADIUS = 15;
extern const int ITERATIONBYONE = 100;
//----------------------------------------------------------------
extern const double SIZE = 1;
void createFolder(const std::string& folderPath) {
  if (!std::filesystem::exists(folderPath)) {
    try {
      std::filesystem::create_directories(folderPath);
    } catch (const std::exception& e) {
      std::cerr << "Error creating folder: " << e.what() << std::endl;
    }
  }
}
int main(int argc, char* argv[]) {
  createFolder(std::to_string(atof(argv[1])) + "Thickness/");
  createFolder(std::to_string(atof(argv[1])) + "Thickness/Spaces/");
  createFolder(std::to_string(atof(argv[1])) + "Thickness/Density/");
  createFolder(std::to_string(atof(argv[1])) + "Thickness/SD/");
  createFolder(std::to_string(atof(argv[1])) + "Thickness/Growth/");
  //  for (int i = atoi(argv[1]); i < atoi(argv[2]); i++) {
  for (int i = 0; i < 1; i++) {
    Surface surface(i, NUMBEROFANGLES, atof(argv[1]));
    double radius = surface.getMeanRadiusOfLivingCells();
    while (radius <= MAXRADIUS) {
      std::cout << i << ": " << radius / MAXRADIUS * 100 << "%\n";
      radius = surface.getMeanRadiusOfLivingCells();
      surface.update(ITERATIONBYONE);
      std::cout << "\033[A\r\33[2K";
    }
    //    surface.saveToFileAllNumberOfCellsEnclosedByRadius("Id" + std::to_string(i) + ".csv");
  }
  return 0;
}
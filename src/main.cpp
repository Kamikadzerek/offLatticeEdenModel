#include <iostream>
#include "Surface.h"
const float NUMBEROFANGLES = 360;
extern const float MAXRADIUS = 275;
extern const int ITERATIONBYONE = 500;
int main(int argc, char* argv[]) {
    for (int i = atoi(argv[2]); i < atoi(argv[3]); i++) {
    Surface surface(i, NUMBEROFANGLES, atof(argv[1]));
    float radius = surface.getMeanRadiusOfLivingCells();
    while (radius <= MAXRADIUS) {
//      std::cout << i << ": " << radius / MAXRADIUS * 100 << "%\n";
      radius = surface.getMeanRadiusOfLivingCells();
      surface.update(ITERATIONBYONE);
//      std::cout << "\033[A\r\33[2K";
    }
        surface.saveToFileAllNumberOfCellsEnclosedByRadius("DENSITY"+std::to_string(atof(argv[1])).substr(0,3)+ "Id" + std::to_string(i) + ".csv");
  }
  return 0;
}
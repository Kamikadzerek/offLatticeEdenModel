#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include "Surface.h"
extern int NUMBEROFLAYERS = 1;
extern const std::string dataPath = std::filesystem::path(std::filesystem::current_path()) /= "../../../dane/MT";
extern const int NUMBEROFANGLES = 180;
int LIMITOFCELLS = 2000;
int ITERATIONBYONE = 10000;
//----------------------------------------------------------------
extern const double WIDTH = 1920 / 2;
extern const double HEIGHT = 1080;
extern const sf::Color EDGE_COLOR = sf::Color(0, 255, 0, 255);
extern const sf::Color TEXT_COLOR = sf::Color(0, 0, 0, 255);
extern const sf::Color BACKGROUND_COLOR = sf::Color(255, 255, 255, 255);
extern const sf::Color ALIVE_COLOR = sf::Color(0, 0, 0, 2 * 255 / NUMBEROFLAYERS);
extern const sf::Color DEAD_COLOR = sf::Color(0, 0, 0, 255 / NUMBEROFLAYERS / 2);
bool DRAWONLYALIVE = false;
bool DRAWCELLS = true;
bool DRAWEDGE = true;
extern const double SIZE = 1;
extern const double OUTLINETHICNESS = 0;
auto start = std::chrono::high_resolution_clock::now();
auto stop = std::chrono::high_resolution_clock::now();
int main(int argc, char *argv[]) {
  //  bool isRunning = true;
  //  bool flag_END_PRINTED = false;
  //  sf::ContextSettings settings;
  //  settings.antialiasingLevel = 4;
  //  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Eden Model", sf::Style::Default, settings);
  //  window.setFramerateLimit(600);
  //  sf::Text leftText;
  //  sf::Text titleText;
  //  sf::Font font;
  //  sf::CircleShape edge;
  //  start = std::chrono::high_resolution_clock::now();
  //  std::cout << "START\n";
  //  window.clear(BACKGROUND_COLOR);
  //  font.loadFromFile("../../src/JetBrainsMono-Bold.ttf");
  //  leftText.setPosition(0, 0);
  //  leftText.setFont(font);
  //  leftText.setCharacterSize(12);
  //  leftText.setFillColor(TEXT_COLOR);
  //  titleText.setPosition(250, 0);
  //  titleText.setFont(font);
  //  titleText.setCharacterSize(25);
  //  titleText.setFillColor(TEXT_COLOR);
  //  Surface surface;
  //  while (window.isOpen()) {
  //    if (surface.getAliveCellsCounter() <= LIMITOFCELLS && isRunning) {
  //      titleText.setString("Eden Model Off-lattice Version C");
  //      surface.update(ITERATIONBYONE);
  //    } else if (!flag_END_PRINTED && isRunning) {
  //      surface.saveToFileAllNumberOfCellsEnclosedByRadius("3MT200000.csv");
  //      std::cout << "END\n";
  //      stop = std::chrono::high_resolution_clock::now();
  //      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  //      std::cout << "Execution time: " + std::to_string(double(duration.count()) / 1000) + "s\n";
  //      flag_END_PRINTED = true;
  //    }
  //    stop = std::chrono::high_resolution_clock::now();
  //    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  //    std::string leftTextStr = "Iteration: " + std::to_string(surface.getIterationCounter()) + "\n" +
  //                              "Alive cells: " + std::to_string(surface.getAliveCellsCounter()) + "\n" +
  //                              "All cells: " + std::to_string(surface.getCells().size()) + "\n" +
  //                              "Execution time: " + std::to_string(double(duration.count()) / 1000) + "s\n";
  //    leftText.setString(leftTextStr);
  //    window.clear(BACKGROUND_COLOR);
  //    window.draw(titleText);
  //    window.draw(leftText);
  //    if (DRAWONLYALIVE) {
  //      for (const auto &cell : surface.getCells()) {
  //        if (cell.getStatus())
  //          window.draw(cell.getDrawable());
  //      }
  //    } else if (DRAWCELLS) {
  //      for (const auto &cell : surface.getCells()) {
  //        window.draw(cell.getDrawable());
  //      }
  //    }
  //    window.display();
  //  }
  for (int i = 30; i < 100; i++) {
    Surface surface(i);
    while (surface.getAliveCellsCounter() <= LIMITOFCELLS) {
      surface.update(ITERATIONBYONE);
    }
    surface.saveToFileAllNumberOfCellsEnclosedByRadius("Id" + std::to_string(i) + ".csv");
    std::cout << i << "\n";
  }
  return 0;
}

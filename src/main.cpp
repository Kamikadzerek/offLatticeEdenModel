#include "Surface.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <random>
#define SQUARES false
extern char VERSION = 'D';
extern const std::string surfacesPath = std::filesystem::path(std::filesystem::current_path()) /= "Saved/Surfaces";
extern const std::string dataPath = std::filesystem::path(std::filesystem::current_path()) /= "../../../dane";
extern const std::string plotsPath = std::filesystem::path(std::filesystem::current_path()) /= "Saved/Plots";
extern const std::string imagesPath = std::filesystem::path(std::filesystem::current_path()) /= "Saved/Images";
// Available Models:
// -Lattice A
// -Lattice B
// -Lattice C
// -Off-Lattice B
// -Off-Lattice C
// Model Off-Lattice A is impossible to implementation because we can't explicitly indicate cells adjacent to the cluster.
extern const int NUMBEROFANGLES = 180;
int LIMITOFCELLS = 100000;
int ITERATIONBYONE = 20000;
//----------------------------------------------------------------
extern const double WIDTH = 1920 / 2;//1920 / 2;
extern const double HEIGHT = 1080;   //1080
extern const sf::Color BACKGROUND_COLOR = sf::Color(249, 219, 189, 255);
extern const sf::Color ALIVE_COLOR = sf::Color(94, 255, 0, 100);
extern const sf::Color EDGE_COLOR = sf::Color(0, 0, 0, 255);
extern const sf::Color DEAD_COLOR = sf::Color(248, 24, 24, 255);
extern const sf::Color TEXT_COLOR = sf::Color(0, 0, 0, 255);
bool DRAWONLYALIVE = false;
bool DRAWCELLS = true;
bool DRAWEDGE = true;
extern const double SIZE = 4;
extern const double OUTLINETHICNESS = 1;
auto start = std::chrono::high_resolution_clock::now();
auto stop = std::chrono::high_resolution_clock::now();
int main(int argc, char *argv[])
{
    bool flag_END_PRINTED = false;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Eden Model", sf::Style::Default, settings);
    sf::Event ev;
    sf::Text leftText;
    sf::Text titleText;
    sf::Font font;
    sf::CircleShape edge;
    start = std::chrono::high_resolution_clock::now();
    std::cout << "START\n";
    window.clear(BACKGROUND_COLOR);
    font.loadFromFile("../../src/JetBrainsMono-Bold.ttf");
    leftText.setPosition(0, 0);
    leftText.setFont(font);
    leftText.setCharacterSize(12);
    leftText.setFillColor(TEXT_COLOR);
    titleText.setPosition(250, 0);
    titleText.setFont(font);
    titleText.setCharacterSize(25);
    titleText.setFillColor(TEXT_COLOR);
#if SQUARES == true
    Surface<sf::RectangleShape> surface;
#else
    //        Surface<sf::CircleShape> surface("Circle200000.csv");
    Surface<sf::CircleShape> surface;
#endif
    while (window.isOpen())
    {

        while (window.pollEvent(ev))
        {
            switch (ev.type)
            {
                case sf::Event::MouseButtonPressed:
                    surface.printCellInfoByClick(ev.mouseButton.x, ev.mouseButton.y);
                    break;
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    if (ev.key.code == sf::Keyboard::Escape)
                        window.close();
                    else if (ev.key.code == sf::Keyboard::R)
                    {
                        surface.clear();
                        start = std::chrono::high_resolution_clock::now();
                    }
                    else if (ev.key.code == sf::Keyboard::S)
                    {
                        surface.saveToFile();
                    }
                    break;
            }
        }
        //        if (surface.getEdgeCells().size() <= LIMITOFCELLS)
        if (surface.getCells().size() <= LIMITOFCELLS)
        //        if(std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()<100)
        {
            if (SQUARES)
            {
                if (VERSION == 'A')
                {
                    titleText.setString("Eden Model Lattice Version A");
                    surface.rectangleUpdateA(ITERATIONBYONE);
                }
                if (VERSION == 'B')
                {
                    titleText.setString("Eden Model Lattice Version B");
                    surface.rectangleUpdateB(ITERATIONBYONE);
                }
                else if (VERSION == 'C')
                {
                    titleText.setString("Eden Model Lattice Version C");
                    surface.rectangleUpdateC(ITERATIONBYONE);
                }
            }
            else
            {
                if (VERSION == 'B')
                {
                    titleText.setString("Eden Model Off-lattice Version B");
                    surface.circleUpdateB(ITERATIONBYONE);
                }
                else if (VERSION == 'C')
                {
                    titleText.setString("Eden Model Off-lattice Version C");
                    surface.circleUpdateC(ITERATIONBYONE);
                }
                else if (VERSION == 'D')
                {
                    titleText.setString("Eden Model Off-lattice Version D");
                    surface.circleUpdateD(ITERATIONBYONE);
                }
            }
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
//            surface.saveToFileTimeOfRunning("CzasJeszczeZadszePomiaryIWizualizacje.csv", double(duration.count()));
            std::cout << LIMITOFCELLS - surface.getCells().size() << "\n";
        }
        else if (!flag_END_PRINTED)
        {
            std::cout << "END\n";
            stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "Execution time: " + std::to_string(double(duration.count()) / 1000) + "s\n";
            flag_END_PRINTED = true;
        }
        stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        leftText.setString("Iteration: " + std::to_string(surface.getIterationCounter()) + "\n" +
                           "Alive cells: " + std::to_string(surface.getAliveCellsCounter()) + "\n" +
                           "All cells: " + std::to_string(surface.getCells().size()) + "\n" +
                           "Execution time: " + std::to_string(double(duration.count()) / 1000) + "s");
        window.clear(BACKGROUND_COLOR);
        window.draw(titleText);
        window.draw(leftText);
        if (DRAWONLYALIVE)
        {
            for (const auto &cell: surface.getCells())
            {
                if (cell.getStatus())
                    window.draw(cell.getDrawable());
            }
        }
        else if (DRAWCELLS)
        {
            for (const auto &cell: surface.getCells())
            {
                window.draw(cell.getDrawable());
            }
        }
        if (DRAWEDGE)
        {
            auto edgeCells = surface.getEdgeCells();
            for (auto *cell: edgeCells)
            {
                cell->setFillColor(sf::Color(100, 100, 100, 255));
                window.draw(cell->getDrawable());
            }
            edge = surface.getEstimateEdge(edgeCells);
            window.draw(edge);
        }
        window.display();
    }
    return 0;
}

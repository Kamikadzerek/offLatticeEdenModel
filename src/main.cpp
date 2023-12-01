#include "Lattice.h"
#include "Surface.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>

// Available Models:
// -Lattice A
// -Lattice cppB
// -Lattice C
// -Off-Lattice B
// -Off-Lattice C
// Model Off-Lattice A is impossible to implementation because we can't explicitly indicate cells adjacent to the cluster.
extern const int NUMBEROFANGLES = 16;
extern const float SIZE = 4;
extern const float OUTLINETHICNESS = 1;
//----------------------------------------------------------------
const double PI = 3.14159265358;
//extern const float DISTANCE = SIZE / 2 * 2.01;
extern const float WIDTH = 1920 / 2;//1920 / 2;
extern const float HEIGHT = 1080;   //1080
extern const sf::Color BACKGROUND_COLOR = sf::Color(249, 219, 189, 255);
extern const sf::Color ALIVE_COLOR = sf::Color(94, 255, 0, 100);
extern const sf::Color EDGE_COLOR = sf::Color(0, 0, 0, 255);
extern const sf::Color DEAD_COLOR = sf::Color(248, 24, 24, 255);
extern const sf::Color TEXT_COLOR = sf::Color(0, 0, 0, 255);
std::string filename = "";
auto start = std::chrono::high_resolution_clock::now();
auto stop = std::chrono::high_resolution_clock::now();

int main(int argc, char *argv[])
{
    bool DRAWONLYALIVE;
    bool LATTICE;
    bool DRAWEDGE;
    char VERSION;
    int LIMITOFCELLS;
    int ITERATIONBYONE;
    bool isSetDOA = false;
    bool isSetDE = false;
    bool isSetL = false;
    bool isSetV = false;
    bool isSetLOC = false;
    bool isSetIBO = false;
    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "-de") == 0)
        {
            DRAWEDGE = (std::strcmp(argv[i + 1], "false") == 0) ? false : true;
            isSetDE = true;
            continue;
        }
        if (std::strcmp(argv[i], "-doa") == 0)
        {
            DRAWONLYALIVE = (std::strcmp(argv[i + 1], "false") == 0) ? false : true;
            isSetDOA = true;
            continue;
        }
        if (std::strcmp(argv[i], "-l") == 0)
        {
            LATTICE = (std::strcmp(argv[i + 1], "false") == 0) ? false : true;
            isSetL = true;
            continue;
        }
        if (std::strcmp(argv[i], "-v") == 0)
        {
            VERSION = *argv[i + 1];
            isSetV = true;
            continue;
        }
        if (std::strcmp(argv[i], "-ibo") == 0)
        {
            ITERATIONBYONE = int(strtol(argv[i + 1], nullptr, 10));
            isSetIBO = true;
            continue;
        }
        if (std::strcmp(argv[i], "-loc") == 0)
        {
            LIMITOFCELLS = int(strtol(argv[i + 1], nullptr, 10));
            isSetLOC = true;
            continue;
        }
    }
    if (!isSetDOA)
        DRAWONLYALIVE = false;
    if (!isSetDE)
        DRAWEDGE = true;
    if (!isSetL)
        LATTICE = false;
    if (!isSetV)
        VERSION = 'C';
    if (!isSetLOC)
        LIMITOFCELLS = 50000;
    if (!isSetIBO)
        ITERATIONBYONE = 1000;
    start = std::chrono::high_resolution_clock::now();
    std::cout << "START\n";
    // Window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Eden Model", sf::Style::Default);
    window.clear(BACKGROUND_COLOR);
    sf::Event ev;
    sf::Text iterationText;
    sf::Text aliveCellsText;
    sf::Text allCellsText;
    sf::Text timeText;
    sf::Text titleText;
    sf::Font font;
    font.loadFromFile("../src/JetBrainsMono-Bold.ttf");
    font.loadFromFile("../../src/JetBrainsMono-Bold.ttf");
    iterationText.setPosition(0, 0);
    iterationText.setFont(font);
    iterationText.setCharacterSize(12);
    iterationText.setFillColor(TEXT_COLOR);
    aliveCellsText.setPosition(0, 13);
    aliveCellsText.setFont(font);
    aliveCellsText.setCharacterSize(12);
    aliveCellsText.setFillColor(TEXT_COLOR);
    allCellsText.setPosition(0, 26);
    allCellsText.setFont(font);
    allCellsText.setCharacterSize(12);
    allCellsText.setFillColor(TEXT_COLOR);
    timeText.setPosition(0, 39);
    timeText.setFont(font);
    timeText.setCharacterSize(12);
    timeText.setFillColor(TEXT_COLOR);
    titleText.setPosition(250, 0);
    titleText.setFont(font);
    titleText.setCharacterSize(25);
    titleText.setFillColor(TEXT_COLOR);
    sf::RectangleShape rectangle;
    rectangle.setPosition(0, 0);
    rectangle.setSize(sf::Vector2f(200, 50));
    rectangle.setFillColor(BACKGROUND_COLOR);
    if (LATTICE)
    {
        Lattice lattice;
        while (window.isOpen())
        {
            //Event polling
            while (window.pollEvent(ev))
            {
                switch (ev.type)
                {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::KeyPressed:
                        if (ev.key.code == sf::Keyboard::Escape)
                            window.close();
                        else if (ev.key.code == sf::Keyboard::R)
                        {
                            lattice.clear();
                            start = std::chrono::high_resolution_clock::now();
                        }
                        break;
                }
            }
            if (lattice.getCells().size() <= LIMITOFCELLS)
            {
                if (VERSION == 'A')
                {
                    titleText.setString("Eden Model Lattice Version A");
                    lattice.updateA(ITERATIONBYONE);
                }
                if (VERSION == 'B')
                {
                    titleText.setString("Eden Model Lattice Version B");
                    lattice.updateB(ITERATIONBYONE);
                }
                else if (VERSION == 'C')
                {
                    titleText.setString("Eden Model Lattice Version C");
                    lattice.updateC(ITERATIONBYONE);
                }
                stop = std::chrono::high_resolution_clock::now();
            }


            //Update
            //Render
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            iterationText.setString("Iteration: " + std::to_string(lattice.getIterationCounter()));
            aliveCellsText.setString("Alive cells: " + std::to_string(lattice.getAliveCellsCounter()));
            allCellsText.setString("All cells: " + std::to_string(lattice.getCells().size()));
            timeText.setString("Execution time: " + std::to_string(float(duration.count()) / 1000) + "s");
            window.clear(BACKGROUND_COLOR);
            window.draw(rectangle);
            window.draw(iterationText);
            window.draw(aliveCellsText);
            window.draw(allCellsText);
            window.draw(timeText);
            window.draw(titleText);
            if (DRAWONLYALIVE)
            {
                for (SqrCell cell: lattice.getCells())
                {
                    if (cell.getStatus())
                        window.draw(cell.getRectangleShape());
                }
            }
            else
            {
                for (SqrCell cell: lattice.getCells())
                {
                    window.draw(cell.getRectangleShape());
                }
            }
            if (DRAWEDGE)
            {
                window.draw(lattice.getEdge());
            }
            window.display();//Tell app that window is done drawing
            //        sf::Texture texture;
            //        texture.create(window.getSize().x, window.getSize().y);
            //        texture.updateA(window);
            //        filename = std::string(5, '0').append(std::to_string(plane.getIterationCounter()));
            //        filename = filename.substr(filename.length() - 5);
            //        filename = filename + ".jpg";
            //        if (texture.copyToImage().saveToFile("../Pictures/" + filename))
            //        {
            //            std::cout << "screenshot saved to " << filename << std::endl;
            //        }
        }
    }
    else
    {
        Surface surface;
        while (window.isOpen())
        {
            //Event polling
            while (window.pollEvent(ev))
            {
                switch (ev.type)
                {
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
                        break;
                }
            }
            if (surface.getCells().size() <= LIMITOFCELLS)
            {
                if (VERSION == 'B')
                {
                    titleText.setString("Eden Model Off-lattice Version B");
                    surface.updateB(ITERATIONBYONE);
                }
                else if (VERSION == 'C')
                {
                    titleText.setString("Eden Model Off-lattice Version C");
                    surface.updateC(ITERATIONBYONE);
                }
                stop = std::chrono::high_resolution_clock::now();
            }


            //Update
            //Render
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            iterationText.setString("Iteration: " + std::to_string(surface.getIterationCounter()));
            aliveCellsText.setString("Alive cells: " + std::to_string(surface.getAliveCellsCounter()));
            allCellsText.setString("All cells: " + std::to_string(surface.getCells().size()));
            timeText.setString("Execution time: " + std::to_string(float(duration.count()) / 1000) + "s");
            window.clear(BACKGROUND_COLOR);
            window.draw(rectangle);
            window.draw(iterationText);
            window.draw(aliveCellsText);
            window.draw(allCellsText);
            window.draw(timeText);
            window.draw(titleText);
            if (DRAWONLYALIVE)
            {
                for (Cell cell: surface.getCells())
                {
                    if (cell.getStatus())
                        window.draw(cell.getCircleShape());
                }
            }
            else
            {
                for (Cell cell: surface.getCells())
                {
                    window.draw(cell.getCircleShape());
                }
            }
            if (DRAWEDGE)
            {
                window.draw(surface.getEdge());
            }
            window.display();//Tell app that window is done drawing
            //        sf::Texture texture;
            //        texture.create(window.getSize().x, window.getSize().y);
            //        texture.updateA(window);
            //        filename = std::string(5, '0').append(std::to_string(plane.getIterationCounter()));
            //        filename = filename.substr(filename.length() - 5);
            //        filename = filename + ".jpg";
            //        if (texture.copyToImage().saveToFile("../Pictures/" + filename))
            //        {
            //            std::cout << "screenshot saved to " << filename << std::endl;
            //        }
        }
    }
    return 0;
}

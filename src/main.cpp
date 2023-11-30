#include "Lattice.h"
#include "Surface.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
// Ultimately parameters will be provided as a program arguments.
const bool DRAWONLYALIVE = false;
// Available Models:
// -Lattice A
// -Lattice cppB
// -Lattice C
// -Off-Lattice B
// -Off-Lattice C
// Model Off-Lattice A is impossible to implementation because we can't explicitly indicate cells adjacent to the cluster.
const bool LATTICE = false;
const char VERSION = 'B';
const int LIMITOFCELLS = 50000;
extern const int NUMBEROFANGLES = 16;
extern const float SIZE = 4;
extern const float OUTLINETHICNESS = 1;
const int ITERATIONBYONE = 100;
//----------------------------------------------------------------
extern const float RADIUS = SIZE / 2;
const double PI = 3.14159265358;
extern const float DISTANCE = RADIUS * 2.01;
extern const float WIDTH = 1920 / 2;//1920 / 2;
extern const float HEIGHT = 1080;   //1080
extern const sf::Color BACKGROUND_COLOR = sf::Color(249, 219, 189, 255);
extern const sf::Color ALIVE_COLOR = sf::Color(94, 255, 0, 100);
extern const sf::Color EDGE_COLOR = sf::Color(0, 0, 0, 255);
extern const sf::Color DEAD_COLOR = sf::Color(248, 24, 24, 255);
extern const sf::Color TEXT_COLOR = sf::Color(0, 0, 0, 255);
std::string filename = "";
sf::Vector2f centerOfMass(const std::vector<Cell> cells)
{
    float sumX = 0;
    float sumY = 0;
    for (Cell cell: cells)
    {
        sumX += cell.getX();
        sumY += cell.getY();
    }
    return sf::Vector2f(sumX / cells.size(), sumY / cells.size());
}
float radiusOfFittedEdge(const std::vector<Cell> cells, const sf::CircleShape edge)
{
    float x = edge.getPosition().x;
    float y = edge.getPosition().y;
    float bestRadius = 0;
    float bestSumSquares = MAXFLOAT;
    for (float testRadius = 1.; testRadius < WIDTH; testRadius += 1)
    {
        float sumSquares = 0;
        for (Cell cell: cells)
        {
            if (cell.getStatus())
            {
                float d = std::sqrt(std::pow(std::abs(x - cell.getX()), 2) + std::pow(std::abs(y - cell.getY()), 2));
                sumSquares += std::pow(d - testRadius, 2);
            }
        }
        if (sumSquares < bestSumSquares)
        {
            bestRadius = testRadius;
            bestSumSquares = sumSquares;
        }
    }
    return bestRadius;
}
sf::CircleShape edge(const std::vector<Cell> cells)
{
    sf::CircleShape edge;
    edge.setPosition(centerOfMass(cells));
    edge.setFillColor(sf::Color(0, 0, 0, 0));
    edge.setOutlineThickness(2);
    edge.setOutlineColor(sf::Color(0, 0, 0, 255));
    edge.setRadius(radiusOfFittedEdge(cells, edge));
    edge.move(-edge.getRadius(), -edge.getRadius());
    return edge;
}

auto start = std::chrono::high_resolution_clock::now();
auto stop = std::chrono::high_resolution_clock::now();

int main()
{
    start = std::chrono::high_resolution_clock::now();
    std::cout << "START\n";
    // Window
    sf::RenderWindow window(sf::VideoMode(1000, 480), "Eden Model", sf::Style::Default);
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
            if(surface.getAliveCellsCounter()>100){
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
    //Game loop
    //End of application
    return 0;
}

#include "Lattice.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <iostream>
#include <random>
bool DRAWONLYALIVE = false;
extern const int NUMBEROFANGLES = 16;
extern const float WIDTH = 1920 / 2;//1920 / 2;
extern const float HEIGHT = 1080;   //1080
extern const float RADIUS = 3;
extern const float DISTANCE = RADIUS * 2.01;
extern const float OUTLINETHICNESS = 2;
const double PI = 3.14159265358;
const int ITERATIONBYONE = 100;
extern const sf::Color BACKGROUND_COLOR = sf::Color(249, 219, 189, 255);
extern const sf::Color ALIVE_COLOR = sf::Color(94, 255, 0, 100);
extern const sf::Color EDGE_COLOR = sf::Color(0, 0, 0, 255);
extern const sf::Color DEAD_COLOR = sf::Color(248, 24, 24, 255);
extern const sf::Color TEXT_COLOR = sf::Color(0, 0, 0, 255);
std::string filename = "";
auto start = std::chrono::high_resolution_clock::now();
auto stop = std::chrono::high_resolution_clock::now();

int main()
{
    start = std::chrono::high_resolution_clock::now();
    std::cout << "START\n";
    // Window
    sf::RenderWindow window(sf::VideoMode(1000, 480), "My first Game", sf::Style::Default);
    window.clear(BACKGROUND_COLOR);
    sf::Event ev;
    sf::Text iterationText;
    sf::Text aliveCellsText;
    sf::Text allCellsText;
    sf::Text timeText;
    sf::Font font;
    font.loadFromFile("../../src/JetBrainsMono-Bold.ttf");
    iterationText.setPosition(0, 0);
    iterationText.setFont(font);
    iterationText.setCharacterSize(12);
    iterationText.setColor(TEXT_COLOR);
    aliveCellsText.setPosition(0, 13);
    aliveCellsText.setFont(font);
    aliveCellsText.setCharacterSize(12);
    aliveCellsText.setColor(TEXT_COLOR);
    allCellsText.setPosition(0, 26);
    allCellsText.setFont(font);
    allCellsText.setCharacterSize(12);
    allCellsText.setColor(TEXT_COLOR);
    timeText.setPosition(0, 39);
    timeText.setFont(font);
    timeText.setCharacterSize(12);
    timeText.setColor(TEXT_COLOR);
    sf::RectangleShape rectangle;
    rectangle.setPosition(0, 0);
    rectangle.setSize(sf::Vector2f(200, 50));
    rectangle.setFillColor(BACKGROUND_COLOR);
    Lattice lattice;
    //Game loop
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
                        lattice.clear();
                    break;
            }
        }

        if (lattice.getCells().size() <= 3005)
        {
            lattice.updateA(ITERATIONBYONE);
            stop = std::chrono::high_resolution_clock::now();
        }



        //Update
        //Render
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        iterationText.setString("Iteration: " + std::to_string(lattice.getIterationCounter()));
        aliveCellsText.setString("Alive cells: " + std::to_string(lattice.getAliveCellsCounter()));
        allCellsText.setString("All cells: " + std::to_string(lattice.getCells().size()));
        timeText.setString("Execution time: " + std::to_string(float(duration.count())/1000) +"s");
        window.clear(BACKGROUND_COLOR);
        window.draw(rectangle);
        window.draw(iterationText);
        window.draw(aliveCellsText);
        window.draw(allCellsText);
        window.draw(timeText);
        if (DRAWONLYALIVE)
        {
            for (Cell cell: lattice.getCells())
            {
                if (cell.getStatus())
                {
                    window.draw(cell.getCircleShape());
                }
            }
        }
        else
        {
            for (Cell cell: lattice.getCells())
            {
                window.draw(cell.getCircleShape());
            }
        }
        window.display();//Tell app that window is done drawing
                         //        sf::Texture texture;
                         //        texture.create(window.getSize().x, window.getSize().y);
                         //        texture.updateA(window);
                         //        filename = std::string(5, '0').append(std::to_string(lattice.getIterationCounter()));
                         //        filename = filename.substr(filename.length() - 5);
                         //        filename = filename + ".jpg";
                         //        if (texture.copyToImage().saveToFile("../Pictures/" + filename))
                         //        {
                         //            std::cout << "screenshot saved to " << filename << std::endl;
                         //        }
    }
    //End of application
    return 0;
}

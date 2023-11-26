#include "Lattice.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <random>
extern const float WIDTH = 1920 / 2;//1920 / 2;
extern const float HEIGHT = 1080;   //1080
extern const float RADIUS = 4;
extern const float DISTANCE = RADIUS * 2.1;
const double PI = 3.14159265358;
const int ITERATIONBYONE = 100;
extern const sf::Color BACKGROUND_COLOR = sf::Color(39, 40, 56, 255);
extern const sf::Color ALIVE_COLOR = sf::Color(235, 148, 134, 255);
extern const sf::Color EDGE_COLOR = sf::Color(165, 28, 48, 255);
std::string filename = "";


int main()
{
    std::cout << "START\n";
    // Window
    sf::RenderWindow window(sf::VideoMode(1000, 480), "My first Game", sf::Style::Default);
    window.clear(BACKGROUND_COLOR);
    sf::Event ev;
    sf::Text iterationText;
    sf::Text aliveCellsText;
    sf::Text allCellsText;
    sf::Font font;
    font.loadFromFile("../../src/JetBrainsMono-Bold.ttf");
    iterationText.setPosition(0, 0);
    iterationText.setFont(font);
    iterationText.setCharacterSize(12);
    aliveCellsText.setPosition(0, 13);
    aliveCellsText.setFont(font);
    aliveCellsText.setCharacterSize(12);
    allCellsText.setPosition(0, 26);
    allCellsText.setFont(font);
    allCellsText.setCharacterSize(12);
    sf::RectangleShape rectangle;
    rectangle.setPosition(0,0);
    rectangle.setSize(sf::Vector2f(200,50));
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
                    else if (ev.key.code == sf::Keyboard::N)
                    {
                    }
                    break;
            }
        }
        lattice.updateA(ITERATIONBYONE);
        //Update
        //Render
        iterationText.setString("Iteration: " + std::to_string(lattice.getIterationCounter()));
        aliveCellsText.setString("Alive cells: " + std::to_string(lattice.getAliveCellsCounter()));
        allCellsText.setString("All cells: " + std::to_string(lattice.getCells().size()));
        window.clear(BACKGROUND_COLOR);
        window.draw(rectangle);
        window.draw(iterationText);
        window.draw(aliveCellsText);
        window.draw(allCellsText);
        for(Cell cell: lattice.getCells()){
            window.draw(cell.getCircleShape());
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
        if(lattice.getCells().size() >= 3005){
//            sf::Event event;
//            while(true){
//                window.waitEvent(event);
//            }
            lattice.clear();
//            window.close();
        }
    }
    //End of application
    return 0;
}

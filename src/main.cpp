#include "Cell.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>
#include <iostream>
const int WIDTH = 1920 / 2;//1920 / 2;
const int HEIGHT = 1080;   //1080
extern const float RADIUS = 4;
const float DISTANCE = RADIUS * 2 + 2;
const double PI = 3.14159265358;
extern const sf::Color BACKGROUND_COLOR = sf::Color(39, 40, 56, 255);
extern const sf::Color ALIVE_COLOR = sf::Color(235, 148, 134, 255);
extern const sf::Color EDGE_COLOR = sf::Color(165, 28, 48, 255);
int counter = 0;
int aliveCellsCounter = 1;
std::string filename = "";

void restart(int *counter,
             std::vector<Cell> *cells,
             std::vector<Cell*> *cellsAlive)
{
    *counter = 0;
    (*cells).clear();
    (*cellsAlive).clear();
    cells->push_back(Cell(float(WIDTH / 2), float(HEIGHT / 2)));
    cellsAlive->push_back(&cells->back());
}


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

    std::vector<Cell> cells;
    std::vector<Cell *> cellsAlive;
    restart(&counter, &cells, &cellsAlive);
    //Game loop
    while (window.isOpen())
    {
        counter++;
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
                        restart(&counter, &cells, &cellsAlive);
                    else if (ev.key.code == sf::Keyboard::N)
                    {
                        std::cout << cells.size() << "\n";
                    }
                    break;
            }
        }

//        Cell *cell = cellsAlive[static_cast<int>((rand() / RAND_MAX) * cellsAlive.size())];
        Cell *cell = &cells[static_cast<int>((rand() / RAND_MAX) * cells.size())];
        while (!(*cell).getStatus())
        {
            srand(std::chrono::system_clock::now().time_since_epoch().count());
            int loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * cells.size());
            cell = &cells[loc];
//            int loc = static_cast<int>((float(rand()) / float(RAND_MAX)) * cellsAlive.size());
//            cell = cellsAlive[loc];
        }
        if (cell->getStatus())
        {
            double randAngle = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * PI * 2;
            std::vector<double> angles({randAngle,
                                        randAngle + (PI * (1. / 4.)),
                                        randAngle + (PI * (2. / 4.)),
                                        randAngle + (PI * (3. / 4.)),
                                        randAngle + (PI * (4. / 4.)),
                                        randAngle + (PI * (5. / 4.)),
                                        randAngle + (PI * (6. / 4.)),
                                        randAngle + (PI * (7. / 4.))});
            std::shuffle(angles.begin(), angles.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
            bool isConflicting = false;
            for (double angle: angles)
            {
                isConflicting = false;

                float x = cell->getX() + DISTANCE * cos(angle);
                float y = cell->getY() + DISTANCE * sin(angle);
                for (Cell cellTemp: cells)
                {
                    if (pow(std::abs(x - cellTemp.getX()), 2) + pow(std::abs(y - cellTemp.getY()), 2) < pow(2 * RADIUS, 2))
                    {
                        isConflicting = true;
                        break;
                    }
                }
                if (!isConflicting)
                {
                    cells.push_back(Cell(x, y));
//                    cellsAlive.push_back(&cells.back());
                    window.draw(cells.back().getCircleShape());
                    aliveCellsCounter++;
                    break;
                }
            }
            if (isConflicting)
            {
                cell->death();
//                cellsAlive.erase(std::remove(cellsAlive.begin(), cellsAlive.end(), cell), cellsAlive.end());

                window.draw(cell->getCircleShape());
                aliveCellsCounter--;
            }
        }
        //Update
        //Render
        iterationText.setString("Iteration: " + std::to_string(counter));
        aliveCellsText.setString("Alive cells: " + std::to_string(aliveCellsCounter));
        allCellsText.setString("All cells: " + std::to_string(cells.size()));
        window.draw(rectangle);
        window.draw(iterationText);
        window.draw(aliveCellsText);
        window.draw(allCellsText);
        window.display();//Tell app that window is done drawing
        sf::Texture texture;
        texture.create(window.getSize().x, window.getSize().y);
        texture.update(window);
        filename = std::string(5, '0').append(std::to_string(counter));
        filename = filename.substr(filename.length() - 5);
        filename = filename + ".jpg";
        if (texture.copyToImage().saveToFile("../Pictures/" + filename))
        {
            std::cout << "screenshot saved to " << filename << std::endl;
        }
        if(aliveCellsCounter >= 500){
            window.close();
        }
    }
    //End of application
    return 0;
}

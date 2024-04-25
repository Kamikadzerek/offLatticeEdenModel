#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
extern const double SIZE;
class Cell {
  private:
  sf::CircleShape drawable;

  private:
  static inline int counter = 0;
  int id;
  bool status;
  double x;
  double y;
  double size;

  public:
  Cell(double X, double Y) {
    id = counter;
    counter++;
    extern const double OUTLINETHICNESS;
    extern const sf::Color EDGE_COLOR;
    extern const sf::Color ALIVE_COLOR;
    status = true;
    x = X;
    y = Y;
    size = SIZE;
    drawable.setPosition(sf::Vector2f(x, y));
    drawable.setFillColor(ALIVE_COLOR);
    drawable.setOutlineColor(EDGE_COLOR);
    drawable.setOutlineThickness(OUTLINETHICNESS);
    drawable.setRadius(SIZE / 2 + OUTLINETHICNESS);
    drawable.setPointCount(100);
  }
  void alive() {
    extern const sf::Color ALIVE_COLOR;
    status = true;
    drawable.setFillColor(ALIVE_COLOR);
  }
  void death() {
    extern const sf::Color DEAD_COLOR;
    status = false;
    drawable.setFillColor(DEAD_COLOR);
  }
  Cell &operator=(Cell cell) {
    status = cell.getStatus();
    x = cell.getX();
    y = cell.getY();
    drawable = cell.getDrawable();
    return *this;
  }
  bool getStatus() const { return status; }
  double getX() const { return x; }
  double getY() const { return y; }
  double getSize() const { return size; }
  int getId() const { return id; }
  sf::CircleShape getDrawable() const { return drawable; }
  static void resetCounter() {
    counter = 0;
  }
  void setFillColor(sf::Color color) {
    drawable.setFillColor(color);
  }
};

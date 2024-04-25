extern const double SIZE;
class Cell {
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
    status = true;
    x = X;
    y = Y;
    size = SIZE;
  }
  void alive() {
    status = true;
  }
  void death() {
    status = false;
  }
  Cell &operator=(Cell cell) {
    status = cell.getStatus();
    x = cell.getX();
    y = cell.getY();
    return *this;
  }
  bool getStatus() const { return status; }
  double getX() const { return x; }
  double getY() const { return y; }
  double getSize() const { return size; }
  int getId() const { return id; }
  static void resetCounter() {
    counter = 0;
  }
};

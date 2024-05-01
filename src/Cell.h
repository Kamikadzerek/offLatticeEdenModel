extern const double SIZE;
class Cell {
  private:
  bool status;
  double x;
  double y;
  double z;
  double size;

  public:
  Cell(double X, double Y, double Z):x(X),y(Y),z(Z),size(SIZE),status(true){}
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
    z = cell.getZ();
    return *this;
  }
  bool getStatus() const { return status; }
  double getX() const { return x; }
  double getY() const { return y; }
  double getZ() const { return z; }
  double getSize() const { return size; }
};

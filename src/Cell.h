class Cell {
  private:
  bool status;
  double x;
  double y;
  double z;

  public:
  Cell(double X, double Y, double Z):x(X),y(Y),z(Z),status(true){}
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
  [[nodiscard]] bool getStatus() const { return status; }
  [[nodiscard]] double getX() const { return x; }
  [[nodiscard]] double getY() const { return y; }
  [[nodiscard]] double getZ() const { return z; }
};

class Cell {
  private:
  bool status;
  float x;
  float y;
  float z;


  public:
  Cell(float X, float Y, float Z):x(X),y(Y),z(Z),status(true){}
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
  [[nodiscard]] float getX() const { return x; }
  [[nodiscard]] float getY() const { return y; }
  [[nodiscard]] float getZ() const { return z; }
};

#include <chrono>
#include <cmath>
#include <fstream>
#include <random>
#include <vector>
#include "Cell.h"
extern const float MAXRADIUS;
extern const int ITERATIONBYONE;
class Surface// store all cells
{
  template<typename _IntType, typename _UniformRandomBitGenerator>
  std::pair<_IntType, _IntType>
  __gen_two_uniform_ints(_IntType __b0, _IntType __b1,
                         _UniformRandomBitGenerator&& __g)
  {
    _IntType __x
        = std::uniform_int_distribution<_IntType>{0, (__b0 * __b1) - 1}(__g);
    return std::make_pair(__x / __b1, __x % __b1);
  }

  template<typename _RandomAccessIterator,
      typename _UniformRandomNumberGenerator>
  void
  shuffle(_RandomAccessIterator __first, _RandomAccessIterator __last,
          _UniformRandomNumberGenerator&& __g)
  {
    // concept requirements
    __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
                                _RandomAccessIterator>)
    __glibcxx_requires_valid_range(__first, __last);

    if (__first == __last)
      return;

    typedef typename std::iterator_traits<_RandomAccessIterator>::difference_type
        _DistanceType;

    typedef typename std::make_unsigned<_DistanceType>::type __ud_type;
    typedef typename std::uniform_int_distribution<__ud_type> __distr_type;
    typedef typename __distr_type::param_type __p_type;

    typedef typename std::remove_reference<_UniformRandomNumberGenerator>::type
        _Gen;
    typedef typename std::common_type<typename _Gen::result_type, __ud_type>::type
        __uc_type;

    const __uc_type __urngrange = __g.max() - __g.min();
    const __uc_type __urange = __uc_type(__last - __first);

    if (__urngrange / __urange >= __urange)
      // I.e. (__urngrange >= __urange * __urange) but without wrap issues.
    {
      _RandomAccessIterator __i = __first + 1;

      // Since we know the range isn't empty, an even number of elements
      // means an uneven number of elements /to swap/, in which case we
      // do the first one up front:

      if ((__urange % 2) == 0)
      {
        __distr_type __d{0, 1};
        std::iter_swap(__i++, __first + __d(__g));
      }

      // Now we know that __last - __i is even, so we do the rest in pairs,
      // using a single distribution invocation to produce swap positions
      // for two successive elements at a time:

      while (__i != __last)
      {
        const __uc_type __swap_range = __uc_type(__i - __first) + 1;

        const std::pair<__uc_type, __uc_type> __pospos =
            __gen_two_uniform_ints(__swap_range, __swap_range + 1, __g);

        std::iter_swap(__i++, __first + __pospos.first);
        std::iter_swap(__i++, __first + __pospos.second);
      }

      return;
    }

    __distr_type __d;

    for (_RandomAccessIterator __i = __first + 1; __i != __last; ++__i)
      std::iter_swap(__i, __first + __d(__g, __p_type(0, __i - __first)));
  }

  template<typename _InputIterator, typename _Tp>
  _GLIBCXX20_CONSTEXPR
  inline _InputIterator
  find(_InputIterator __first, _InputIterator __last,
       const _Tp& __val)
  {
    // concept requirements
    __glibcxx_function_requires(_InputIteratorConcept<_InputIterator>)
    __glibcxx_function_requires(_EqualOpConcept<
                                typename iterator_traits<_InputIterator>::value_type, _Tp>)
    __glibcxx_requires_valid_range(__first, __last);
    return std::__find_if(__first, __last,
                          __gnu_cxx::__ops::__iter_equals_val(__val));
  }
  private:
  std::vector<std::vector<int>> conflictQueue = {
      std::vector<int>({0, 0, 0}),
      std::vector<int>({1, 0, 0}),
      std::vector<int>({-1, 0, 0}),
      std::vector<int>({0, 1, 0}),
      std::vector<int>({0, -1, 0}),
      std::vector<int>({0, 0, 1}),
      std::vector<int>({0, 0, -1}),
      std::vector<int>({1, 1, 0}),
      std::vector<int>({1, -1, 0}),
      std::vector<int>({1, 0, 1}),
      std::vector<int>({1, 0, -1}),
      std::vector<int>({-1, 1, 0}),
      std::vector<int>({-1, -1, 0}),
      std::vector<int>({-1, 0, 1}),
      std::vector<int>({-1, 0, -1}),
      std::vector<int>({0, 1, 1}),
      std::vector<int>({0, 1, -1}),
      std::vector<int>({0, -1, 1}),
      std::vector<int>({0, -1, -1}),
      std::vector<int>({1, 1, 1}),
      std::vector<int>({1, 1, -1}),
      std::vector<int>({1, -1, 1}),
      std::vector<int>({1, -1, -1}),
      std::vector<int>({-1, 1, 1}),
      std::vector<int>({-1, 1, -1}),
      std::vector<int>({-1, -1, 1}),
      std::vector<int>({-1, -1, -1})};
  int treeSize = MAXRADIUS * 4;
  int halfOfTreeSize = MAXRADIUS * 2;
  int treeSizeZ;
  int halfOfTreeSizeZ;
  std::vector<std::vector<std::vector<std::vector<Cell *>>>> tree;
  int densityOfMeasurements = 100;
  float lastMeanRadius = 0;
  float currentMeanRadius = 0;
  struct angle {
    float sin;
    float cos;
    angle(float sin, float cos):sin(sin),cos(cos){}
  };
  struct vector2d {
    float x;
    float y;
    vector2d(float x, float y):x(x),y(y){}
  };
  std::vector<Cell> cells;
  std::vector<Cell *> aliveCells;
  std::vector<angle> angles;
  float initialX = 0;
  float initialY = 0;
  float initialZ = 0;
  int id;
  int numOfAngles;
  int stateCounter = 0;
  int iterationCounter = 0;
  int aliveCellsCounter;
  float thickness;
  float halfOfThickness;
  float spawnDistance = 1;
  float cellRadius = spawnDistance / 2;
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<float> dis;
  [[nodiscard]] bool circleIsOverCircle(const Cell &cell1, const Cell cell2) const {
    float distZ = cell1.getZ() - cell2.getZ();
    float distX = cell1.getX() - cell2.getX();
    float distY = cell1.getY() - cell2.getY();
    if (std::abs(distX) < 1) {
      if (std::abs(distZ) < 1) {
        //        if (dstBtw2Pts(cell1, cell2) < 1) {
        if (pow(distX, 2) + pow(distY, 2) + pow(distZ, 2) < 1) {
          return true;
        }
      }
    }
    return false;
  }

  public:
  explicit Surface(int i, int numberOfAngles, float thick) : id(i), numOfAngles(numberOfAngles), thickness(thick) {
    treeSizeZ = 5 + thickness * 3;
    halfOfTreeSizeZ = treeSizeZ / 2;
    halfOfThickness = thickness / 2;
    gen = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
    dis = std::uniform_real_distribution<float>(0.0, 1.0);
    aliveCellsCounter = 1;
    cells.reserve(pow(MAXRADIUS / cellRadius, 2) * (thickness + 1) + 2 * ITERATIONBYONE);
    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / numOfAngles) {
      angles.emplace_back(sin(angle), cos(angle));
    }
    for (int i = 0; i < treeSize; i++) {
      tree.emplace_back();
      for (int j = 0; j < treeSize; j++) {
        tree.back().emplace_back();
        for (int k = 0; k < treeSizeZ; k++) {
          tree.back().back().emplace_back();
          tree.back().back().back().reserve(1);
        }
      }
    }
    addCell(initialX, initialY, initialZ);
  }
  ~Surface() {
    cells.clear();
  }
  void addCell(float x, float y, float z) {
    cells.emplace_back(x, y, z);
    aliveCells.emplace_back(&cells.back());
    aliveCellsCounter++;
    tree[std::floor(x) + halfOfTreeSize][std::floor(y) + halfOfTreeSize][std::floor(z) + halfOfTreeSizeZ].push_back(&cells.back());
  }
  void deadCell(Cell *c) {
    c->death();
    auto it = find(aliveCells.begin(), aliveCells.end(), c);
    if (it != aliveCells.end()) {
      std::swap(*it, aliveCells.back());
      aliveCells.pop_back();
    }
    aliveCellsCounter--;
  }
  bool cellIsConflicting(const Cell cell) {
    int x = std::floor(cell.getX()) + halfOfTreeSize;
    int y = std::floor(cell.getY()) + halfOfTreeSize;
    int z = std::floor(cell.getZ()) + halfOfTreeSizeZ;
    for (auto &dir : conflictQueue) {
      if (!tree[x + dir[0]][y + dir[1]][z + dir[2]].empty()) {
        for (auto const &cellTemp : tree[x + dir[0]][y + dir[1]][z + dir[2]]) {
          if (circleIsOverCircle(*cellTemp, cell)) {
            return true;
          }
        }
      }
    }
    return false;
  }
  vector2d getCenterOfMassFromLiving() {
    float sumX = 0;
    float sumY = 0;
    int counter = 0;
    for (const Cell *cell : aliveCells) {
      counter++;
      sumX += cell->getX();
      sumY += cell->getY();
    }
    return vector2d(sumX / counter, sumY / counter);
  }
  [[maybe_unused]] [[nodiscard]] int getAliveCellsCounter() const { return aliveCellsCounter; }
  void update(int numberOfIteration) {
    float halfOfThicknessTrue = halfOfThickness-0.5;
    float x = 0;
    float y = 0;
    float z = 0;
    for (int i = 0; i < numberOfIteration; i++) {
      iterationCounter++;
      unsigned long index = gen() % aliveCells.size();
      Cell *cell = aliveCells[index];
      if (cell->getStatus()) {
        bool isConflicting;
        if (thickness > 1) {
          // 3D VERSION -------------------------------------------------
          if (i % 2 == 0) {
          }
          shuffle(angles.begin(), angles.end(), gen);
          auto theta = angles;
          shuffle(angles.begin(), angles.end(), gen);
          auto phi = angles;
          for (const angle &angle1 : theta) {
            z = cell->getZ() + angle1.sin;
            isConflicting = z > halfOfThicknessTrue || z < -halfOfThicknessTrue;
            if (!isConflicting) {
              for (const angle &angle2 : phi) {
                x = cell->getX() + angle1.cos * angle2.cos;
                y = cell->getY() + angle1.cos * angle2.sin;
                isConflicting = cellIsConflicting(Cell(x, y, z));
                if (!isConflicting) {
                  addCell(x, y, z);
                  break;
                }
              }
            }
          }
        } else {
          // 2D VERSION -------------------------------------------------
          shuffle(angles.begin(), angles.end(), gen);
          for (const angle &angle : angles) {
            float x = cell->getX() + angle.cos;
            float y = cell->getY() + angle.sin;
            float z = 0;
            isConflicting = cellIsConflicting(Cell(x, y, z));
            if (!isConflicting) {
              addCell(x, y, z);
              break;
            }
          }
        }
        // COMMON PART -------------------------------------------------
        if (isConflicting) {
          deadCell(cell);
        }
      }
      if (iterationCounter % densityOfMeasurements == 0) {
        currentMeanRadius = getMeanRadiusOfLivingCells();
        if (currentMeanRadius > lastMeanRadius + 2) {
          lastMeanRadius = currentMeanRadius;
          saveToFileMeanRadiusOfLivingCellsNumOfLivingCells("MRNLC"+std::to_string(thickness).substr(0,3)+ "Id" + std::to_string(id) + ".csv");
          saveToFileSDMean("SD"+std::to_string(thickness).substr(0,3)+ "Id" + std::to_string(id) + ".csv");
        }
      }
    }
  }
  static float dstBtw2Pts(const Cell c1, const Cell c2) {
    return sqrt(pow(c1.getX() - c2.getX(), 2) + pow(c1.getY() - c2.getY(), 2) + pow(c1.getZ() - c2.getZ(), 2));
  }
  static float dstBtw2Pts(const vector2d v, const Cell c) {
    return sqrt(pow(v.x - c.getX(), 2) + pow(v.y - c.getY(), 2));
  }
  static float dstBtw2Pts(const vector2d v, const Cell *c) {
    return sqrt(pow(v.x - c->getX(), 2) + pow(v.y - c->getY(), 2));
  }
  float getMeanRadiusOfLivingCells() {
    int counter = 0;
    float sum = 0;
    const vector2d center = getCenterOfMassFromLiving();
    for (const Cell *cell : aliveCells) {
      counter++;
      sum += dstBtw2Pts(center, cell);
    }
    return sum / counter;
  }
  int getNumberOfCellsEnclosedByRadius(float radius) {
    int counter = 0;
    vector2d center = getCenterOfMassFromLiving();
    for (const Cell cell : cells) {
      if (dstBtw2Pts(center, cell) <= radius) {
        counter++;
      }
    }
    return counter;
  }
  [[maybe_unused]] void saveToFileAllNumberOfCellsEnclosedByRadius(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = fileName;
    fout.open(fullPath);
    float R = getMeanRadiusOfLivingCells() + 100;
    float step = 5;
    for (float r = step; r <= R; r += step) {
      fout << std::pow(r / cellRadius, 2) << "\t" << getNumberOfCellsEnclosedByRadius(r) << "\n";
    }
    fout.close();
  }
  [[maybe_unused]] void saveToFileMeanRadiusOfLivingCellsNumOfLivingCells(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = fileName;
    fout.open(fullPath, std::ios_base::app);
    fout << /*2 * */  getMeanRadiusOfLivingCells() / cellRadius << "\t" << aliveCellsCounter << "\n";// poprawka błąd w pracy USUNIETA
    fout.close();
  }
  float getSD() {
    float meanRadius = getMeanRadiusOfLivingCells();
    const vector2d center = getCenterOfMassFromLiving();
    std::vector<float> deviations;
    for (const Cell *cell : aliveCells) {
      deviations.push_back(pow(meanRadius - dstBtw2Pts(center, cell), 2));
    }
    float variance = 0;
    for (float v : deviations) {
      variance += v;
    }
    variance /= deviations.size();
    return sqrt(variance) / cellRadius;
  }
  void saveToFileSDMean(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = fileName;
    fout.open(fullPath, std::ios_base::app);
    fout << getMeanRadiusOfLivingCells() / cellRadius << "\t" << getSD() << "\n";
    fout.close();
  }
  [[maybe_unused]] void saveToFile(const std::string &fileName) {
    std::ofstream fout;
    std::string fullPath = fileName;
    fout.open(fullPath);
    for (auto const cell : cells) {
      fout << cell.getX() * 10 << "\t" << cell.getY() * 10 << "\t" << cell.getZ() * 10 << "\t";
      if (cell.getStatus()) {
        fout << "1";
      } else {
        fout << "0";
      }
      fout << "\n";
    }
    fout.close();
  }
};

#include "helper_functions.h"

#include <math.h>
#include <algorithm>
#include <iostream>

using std::list;
using std::vector;

list<int> GetDivisors(int num) {
  list<int> divisors;
  for (int i = 1; i <= sqrt(num); ++i) {
    if (num % i == 0) {
      divisors.push_back(i);
    }
  }
  return divisors;
}

vector<Shape> CalculateShapes(int min_area, int max_area) {
  vector<Shape> shapes;
  for (auto area = min_area; area <= max_area; ++area) {
    const auto divisors = GetDivisors(area);
    for (auto div : divisors) {
      const auto second_side = area / div;
      shapes.push_back({div, second_side});
      if (div != second_side) {
        shapes.push_back({second_side, div});
      }
    }
  }
  std::sort(shapes.begin(), shapes.end(),
            [](const Shape& shape1, const Shape& shape2) {
              return std::abs(shape1.width - shape1.height) <
                     std::abs(shape2.width - shape2.height);
            });
  return shapes;
}

Slice SliceForShape(const Shape& shape, int row, int col) {
  return {row, col, row + shape.width - 1, col + shape.height - 1};
}

vector<vector<int>> CalculateCellSums(const vector<vector<Cell>>& cells) {
  const int r = cells.size();
  const int c = cells[0].size();
  vector<vector<int>> cells_sum(r, vector<int>(c));

  for (int i = 0; i < c; i++) {
    cells_sum[0][i] = cells[0][i].val;
  }

  for (int i = 1; i < r; i++)
    for (int j = 0; j < c; j++) {
      cells_sum[i][j] = cells[i][j].val + cells_sum[i - 1][j];
    }

  for (int i = 0; i < r; i++)
    for (int j = 1; j < c; j++) {
      cells_sum[i][j] += cells_sum[i][j - 1];
    }

  return cells_sum;
}

int GetCellsSum(const vector<vector<int>>& cells_sum, const Slice& slice) {
  int res = cells_sum[slice.r2][slice.c2];
  if (slice.r1 > 0) {
    // Remove elements from above.
    res -= cells_sum[slice.r1 - 1][slice.c2];
  }
  if (slice.c1 > 0) {
    // Remove element from the left.
    res -= cells_sum[slice.r2][slice.c1 - 1];
  }
  if (slice.r1 > 0 && slice.c1 > 0) {
    // Add back elements that were removed twice.
    res += cells_sum[slice.r1 - 1][slice.c1 - 1];
  }
  return res;
}

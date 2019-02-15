#include "helper_functions.h"

#include <algorithm>
#include "assert.h"
#include <iostream>
#include <math.h>

using std::experimental::nullopt;
using std::experimental::optional;
using std::list;
using std::vector;

namespace {

optional<Slice> MakeHorizontalCut(const Slice& cutted_slice,
                                  const Slice& cutting_slice) {
  const bool has_proper_width = cutting_slice.c1 <= cutted_slice.c1 &&
                                cutting_slice.c2 >= cutted_slice.c2;
  const bool cut_top =
      cutting_slice.r1 <= cutted_slice.r1 && cutting_slice.r2 < cutted_slice.r2;
  const bool cut_bottom =
      cutted_slice.r1 > cutted_slice.r1 && cutted_slice.r2 >= cutted_slice.r2;

  if (!has_proper_width || !(cut_top || cut_bottom)) {
    return nullopt;
  }

  int r1_offset = 0;
  int r2_offset = 0;

  if (cut_top) {
    r1_offset = cutting_slice.r2 - cutted_slice.r1 + 1;
  } else {
    assert(cut_bottom);
    r2_offset = cutted_slice.r2 - cutting_slice.r1 + 1;
  }
  return Slice{cutted_slice.r1 + r1_offset, cutted_slice.c1,
               cutted_slice.r2 - r2_offset, cutted_slice.c2};
}

Slice Rotate(const Slice& slice) {
  return Slice{slice.c1, slice.r1, slice.c2, slice.r2};
}

}  // namespace

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

optional<Slice> CutSlice(const Slice& cutted_slice,
                         const Slice& cutting_slice) {
  if (auto slice = MakeHorizontalCut(cutted_slice, cutting_slice))
    return slice;

  if (auto slice =
          MakeHorizontalCut(Rotate(cutted_slice), Rotate(cutting_slice)))
    return Rotate(*slice);

  return nullopt;
}

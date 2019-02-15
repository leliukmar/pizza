#include "solution.h"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <math.h>
#include <set>
#include <utility>

#include "common/helper_functions.h"

using std::list;
using std::experimental::nullopt;
using std::experimental::optional;
using std::vector;

namespace {

class ScopedTimer {
 public:
  ScopedTimer(std::string str) : name_(std::move(str)) {
    start_ = std::clock();
  }

  ~ScopedTimer() {
    std::cout << name_ << "). Time passed: "
              << (std::clock() - start_) / (double)(CLOCKS_PER_SEC / 1000)
              << " ms" << std::endl;
  }

 private:
  std::clock_t start_;
  std::string name_;
};

enum Direction { kRight, kDown, kLeft, kTop };

Slice GetExtendedSlice(const Slice& slice, Direction dir) {
  Slice extended_slice = slice;
  switch (dir) {
    case kRight:
      extended_slice.c2 += 1;
      break;
    case kDown:
      extended_slice.r2 += 1;
      break;
    case kLeft:
      extended_slice.c1 -= 1;
    case kTop:
      extended_slice.r1 -= 1;
      break;
  }
  return extended_slice;
}

bool SlicesIntersect(const Slice& slice1, const Slice& slice2) {
  return !(slice2.c1 >= slice1.c2 ||
           slice2.c2 <= slice1.c1 || slice2.r1 >= slice1.r2 || slice2.r2 <= slice1.r1);
}

}  // namespace

namespace greedy {

Solution::Solution(Pizza pizza)
    : pizza_(std::move(pizza)),
      shapes_(CalculateShapes(pizza_.min_ingred()*2, pizza_.min_ingred()*2)) {}

void Solution::Solve() {
  // First find fiting slices for each position in pizza.
  for (int i = 0; i < pizza_.rows(); ++i) {
    for (int j = 0; j < pizza_.columns(); ++j) {
      FindFittingSlice(i, j);
    }
  }

  // Try to extend each slice as much as possible;
  for (auto i = 0; i < pizza_.slices().size(); ++i) {
    if (auto extended_slice = ExtendSlice(
            pizza_.slices()[i], i, false /*exis_on_first_success*/, true)) {
      pizza_.ReplaceSlice(*extended_slice, i);
    }
  }
}

void Solution::FindFittingSlice(int row, int col) {
  // First just try all min shapes.
  for (const auto& shape : shapes_) {
    const auto slice = SliceForShape(shape, row, col);
    if (pizza_.IsValidSlice(slice, nullopt)) {
      pizza_.AddSlice(slice);
      return;
    }
  }

  // Try extend min shapes.
  for (const auto& shape : shapes_) {
    if (auto slice = ExtendSlice(SliceForShape(shape, row, col), nullopt,
                                 true /*exit_on_first_success*/, false)) {
      pizza_.AddSlice(*slice);
      return;
    }
  }
}

optional<Slice> Solution::ExtendSlice(
    const Slice& slice, std::experimental::optional<int> slice_idx,
    bool exit_on_first_success, bool occupy_neighbours) {
  optional<Slice> extended_slice;
  while (true) {
    optional<Slice> possible_slice;
    for (auto dir : {kRight, kDown, kLeft, kTop}) {
      auto slice_to_try = GetExtendedSlice(extended_slice.value_or(slice), dir);

      if (!pizza_.PizzaContains(slice_to_try) ||
          pizza_.SliceIsTooBig(slice_to_try))
        continue;

      if (pizza_.SliceIntersectsOther(slice_to_try, slice_idx) &&
          (!occupy_neighbours ||
           !CanOccupyNeighbours(slice_to_try, slice_idx))) {
        continue;
      }

      if (pizza_.HasEnoughIngredients(slice_to_try) && exit_on_first_success) {
        return slice_to_try;
      }

      possible_slice = slice_to_try;
    }

    if (!possible_slice) break;

    // If some slice fits -> extend it even further.
    extended_slice = possible_slice;
  }

  return extended_slice
             ? (pizza_.HasEnoughIngredients(*extended_slice) ? extended_slice
                                                             : nullopt)
             : extended_slice;
}

bool Solution::CanOccupyNeighbours(const Slice& slice,
                                   const optional<int>& slice_idx) {
  std::set<int> ids_to_cut;
  for (int i = slice.r1; i <= slice.r2; ++i) {
    for (int j = slice.c1; j <= slice.c2; ++j) {
      const auto existing_slice_id = pizza_.cells()[i][j].slice_id;
      if (existing_slice_id && existing_slice_id != slice_idx) {
        ids_to_cut.insert(*existing_slice_id);
      }
    }
  }

  for (auto id_to_cut : ids_to_cut) {
    if (auto cutted_slice = CutSlice(pizza_.slices()[id_to_cut], slice)) {
      if (pizza_.HasEnoughIngredients(*cutted_slice))
        continue;
    }
    return false;
  }

  return true;
}

}  // namespace greedy

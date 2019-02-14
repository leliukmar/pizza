#include "solution.h"

#include <algorithm>
#include <iostream>
#include <math.h>

#include "common/helper_functions.h"

using std::list;
using std::experimental::nullopt;
using std::experimental::optional;
using std::vector;

namespace {

enum Direction { kRight, kDown, kLeft, kTop };

Slice GetExtendedSlice(const Slice& slice, Direction dir) {
  auto ret_val = slice;
  switch (dir) {
    case kRight:
      ret_val.c2 += 1;
      break;
    case kDown:
      ret_val.r2 += 1;
      break;
    case kLeft:
      ret_val.c1 -= 1;
    case kTop:
      ret_val.r1 -= 1;
      break;
  }
  return ret_val;
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

void Solution::solve() {
  // First find fiting slices for each position in pizza.
  for (int i = 0; i < pizza_.rows(); ++i) {
    for (int j = 0; j < pizza_.columns(); ++j) {
      FindFittingSlice(i, j);
    }
  }

  // Try to extend each slice as much as possible;
  for (auto i = 0; i < pizza_.slices().size(); ++i) {
    if (auto extended_slice =
            ExtendSlice(pizza_.slices()[i], i, false /*exis_on_first_success*/)) {
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
    if (auto slice = GetMinExtededSlice(SliceForShape(shape, row, col))) {
      pizza_.AddSlice(*slice);
      return;
    }
  }
}

optional<Slice> Solution::GetMinExtededSlice(const Slice& slice) {
  if (!pizza_.PizzaContains(slice) || pizza_.SliceIsTooBig(slice) ||
      pizza_.SliceIntersectsOther(slice, nullopt)) {
    return nullopt;
  }

  if (pizza_.HasEnoughIngredients(slice)) return slice;

  return ExtendSlice(slice, nullopt, true /*exit_on_first_success*/);
}

optional<Slice> Solution::ExtendSlice(
    const Slice& slice, std::experimental::optional<int> slice_idx,
    bool exit_on_first_success) {
  // TODO(leliukmar): occupy neighbours cells if neighbour cell will be ok with
  // it.
  optional<Slice> extended_slice;
  while (true) {
    optional<Slice> possible_slice;
    for (auto dir : {kRight, kDown, kLeft, kTop}) {
      auto slice_to_try = GetExtendedSlice(extended_slice.value_or(slice), dir);

      if (!pizza_.PizzaContains(slice_to_try) ||
          pizza_.SliceIsTooBig(slice_to_try) ||
          pizza_.SliceIntersectsOther(slice_to_try, slice_idx))
        continue;

      if (pizza_.HasEnoughIngredients(slice_to_try) && exit_on_first_success)
        return slice_to_try;

      possible_slice = slice_to_try;
    }

    if (!possible_slice) break;

    // If some slice fits -> extend it even further.
    extended_slice = possible_slice;
  }

  return extended_slice
             ? (pizza_.IsValidSlice(*extended_slice, slice_idx) ? extended_slice
                                                                : nullopt)
             : extended_slice;
}

}  // namespace greedy

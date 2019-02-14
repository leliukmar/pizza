#include "pizza.h"

#include <math.h>
#include <algorithm>
#include <iostream>

#include "helper_functions.h"

using std::list;
using std::experimental::nullopt;
using std::experimental::optional;
using std::vector;

Slice::Slice(int r1, int c1, int r2, int c2) : r1(r1), c1(c1), r2(r2), c2(c2) {}

Pizza::Pizza(std::vector<std::vector<Cell>> cells, int min_ingred, int max_area)
    : cells_(std::move(cells)),
      cells_sum_(CalculateCellSums(cells_)),
      min_ingred_(min_ingred),
      max_area_(max_area) {}

void Pizza::AddSlice(const Slice& slice) {
  slices_.push_back(slice);
  const int idx = slices_.size() - 1;
  for (int i = slice.r1; i <= slice.r2; ++i) {
    for (int j = slice.c1; j <= slice.c2; ++j) {
      cells_[i][j].slice_id = idx;
    }
  }
}

void Pizza::ReplaceSlice(const Slice& new_slice, int idx) {
  slices_[idx] = new_slice;
  OccupyCellsBySlice(idx);
}

bool Pizza::PizzaContains(const Slice& slice) const {
  return slice.r1 >= 0 && slice.r2 < cells_.size() && slice.c1 >= 0 &&
         slice.c2 < cells_[0].size();
}

bool Pizza::HasEnoughIngredients(const Slice& slice) const {
  const int cells_sum = GetCellsSum(cells_sum_, slice);
  return cells_sum >= min_ingred_ && cells_sum <= (slice.area() - min_ingred_);
}

bool Pizza::SliceIsTooBig(const Slice& slice) const {
  return slice.area() > max_area_;
}

bool Pizza::SliceIntersectsOther(const Slice& slice,
                                 optional<int> slice_idx) const {
  for (int i = slice.r1; i <= slice.r2; ++i) {
    for (int j = slice.c1; j <= slice.c2; ++j) {
      if (cells_[i][j].slice_id && (cells_[i][j].slice_id != slice_idx)) {
        return true;
      }
    }
  }
  return false;
}

bool Pizza::IsValidSlice(const Slice& slice, optional<int> slice_idx) const {
  return PizzaContains(slice) && !SliceIsTooBig(slice) &&
         HasEnoughIngredients(slice) && !SliceIntersectsOther(slice, slice_idx);
}

void Pizza::OccupyCellsBySlice(int slice_idx) {
  const Slice& slice = slices_[slice_idx];
  for (int i = slice.r1; i <= slice.r2; ++i) {
    for (int j = slice.c1; j <= slice.c2; ++j) {
      cells_[i][j].slice_id = slice_idx;
    }
  }
}

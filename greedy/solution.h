#ifndef GREEDY_SOLUTION_H
#define GREEDY_SOLUTION_H

#include "common/pizza.h"

namespace greedy {

class Solution {
 public:
  explicit Solution(Pizza pizza);

  void Solve();
  const Pizza& pizza() const { return pizza_; }

 private:
  void FindFittingSlice(int row, int col);
  std::experimental::optional<Slice> ExtendSlice(
      const Slice& slice, std::experimental::optional<int> slice_idx,
      bool exit_on_first_success, bool occupy_neighbours);
  bool CanOccupyNeighbours(const Slice& slice,
                           const std::experimental::optional<int>& slice_idx);

  Pizza pizza_;
  const std::vector<Shape> shapes_;
};

}  // namespace greedy

#endif  // GREEDY_SOLUTION_H

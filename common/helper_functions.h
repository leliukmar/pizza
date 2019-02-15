#ifndef COMMON_HELPER_FUNCTIONS_H
#define COMMON_HELPER_FUNCTIONS_H

#include "pizza.h"

std::vector<std::vector<int>> CalculateCellSums(
    const std::vector<std::vector<Cell>>& cells);
int GetCellsSum(const std::vector<std::vector<int>>& cells_sum,
                const Slice& slice);

std::list<int> GetDivisors(int num);
std::vector<Shape> CalculateShapes(int min_ingrid, int max_area);

Slice SliceForShape(const Shape& shape, int row, int col);

std::experimental::optional<Slice> CutSlice(const Slice& cutted_slice,
                                            const Slice& cutting_slice);

#endif  // COMMON_HELPER_FUNCTIONS_H

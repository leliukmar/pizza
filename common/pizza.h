#ifndef COMMON_PIZZA_H
#define COMMON_PIZZA_H

#include <stdint.h>
#include <experimental/optional>
#include <list>
#include <string>
#include <vector>

struct Shape {
  int area() const { return width * height; }

  int width;
  int height;
};

struct Cell {
  uint8_t val;
  std::experimental::optional<int> slice_id;
};

struct Slice {
  Slice(int r1, int c1, int r2, int c2);

  int width() const { return c2 - c1 + 1; }
  int height() const { return r2 - r1 + 1; }
  int area() const { return (r2 - r1 + 1) * (c2 - c1 + 1); }

  std::string ToString() const {
    return std::to_string(r1) + " " + std::to_string(c1) + " " +
           std::to_string(r2) + " " + std::to_string(c2);
  }

  int r1, c1, r2, c2;
};

class Pizza {
 public:
  static const uint8_t kTomato = 0;
  static const uint8_t kMarshroom = 1;

  Pizza(std::vector<std::vector<Cell>> cells, int min_ingred, int max_area);

  void AddSlice(const Slice& slice);
  void ReplaceSlice(const Slice& new_slice, int idx);

  bool PizzaContains(const Slice& slice) const;
  bool HasEnoughIngredients(const Slice& slice) const;
  bool SliceIsTooBig(const Slice& slice) const;

  // Slice might not be added to pizza yet, so its index will be |nullopt|.
  bool SliceIntersectsOther(const Slice& slice,
                            std::experimental::optional<int> slice_idx) const;
  // Checks everything.
  bool IsValidSlice(const Slice& slice,
                    std::experimental::optional<int> slice_idx) const;

  const std::vector<Slice>& slices() const { return slices_; }
  const std::vector<std::vector<Cell>> cells() const { return cells_; }
  int rows() const { return cells_.size(); }
  int columns() const { return cells_[0].size(); }
  int min_ingred() const { return min_ingred_; }
  int max_area() const { return max_area_; }

 private:
  void OccupyCellsBySlice(int slice_idx);

  int min_ingred_;
  int max_area_;
  std::vector<std::vector<Cell>> cells_;
  const std::vector<std::vector<int>> cells_sum_;
  std::vector<Slice> slices_;
};

#endif  // COMMON_PIZZA_H

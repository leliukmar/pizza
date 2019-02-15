#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "common/pizza.h"
#include "greedy/solution.h"
//#include "max_weigh_graph/solution.h"

using std::cin;
using std::cout;
using std::experimental::nullopt;
using std::vector;

void VerifySolution(vector<vector<Cell>> initial_cells,
                    const vector<Slice>& slices, int l, int h, int r, int c) {
  for (int idx = 0; idx < slices.size(); ++idx) {
    const auto& slice = slices[idx];
    if (slice.area() > h) {
      std::cout << "Verification error: too big. " << slice.ToString()
                << std::endl;
    }

    if (slice.r1 < 0 || slice.r2 >= r || slice.c1 < 0 || slice.c2 >= c) {
      std::cout << "Verification error: slice is outside pizza bounds." << std::endl;
    }

    int tomato = 0;
    int marshrooms = 0;
    for (int i = slice.r1; i <= slice.r2; ++i) {
      for (int j = slice.c1; j <= slice.c2; ++j) {
        if (initial_cells[i][j].slice_id) {
          std::cout << "Verification error: intersection found "
                    << slice.ToString() << " and " << slices[*initial_cells[i][j].slice_id].ToString()
                    << ", idx: " << idx << ", " << *initial_cells[i][j].slice_id << std::endl;
        } else {
          initial_cells[i][j].slice_id = idx;
        }

        if (initial_cells[i][j].val == 0) {
          ++tomato;
        } else {
          ++marshrooms;
        }
      }
    }
    if (tomato < l || marshrooms < l) {
      std::cout << "Verification error: not enought ingridients "
                << slice.ToString() << ", t = " << tomato
                << ", m = " << marshrooms << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Format error: input_file output_file" << std::endl;
    return -1;
  }

  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  int r, c, l, h;
  input >> r >> c >> l >> h;

  vector<vector<Cell>> initial_cells(r, vector<Cell>(c));
  char cell_type;
  for (int ri = 0; ri < r; ++ri) {
    for (int ci = 0; ci < c; ++ci) {
      input >> cell_type;
      if (cell_type == 'T') {
        initial_cells[ri][ci] = {Pizza::kTomato, nullopt};
      } else if (cell_type == 'M') {
        initial_cells[ri][ci] = {Pizza::kMarshroom, nullopt};
      } else {
        std::cout << "Error no such type: " << cell_type;
        return 1;
      }
    }
  }

  greedy::Solution solution(Pizza(initial_cells, l, h));
  solution.Solve();
  const auto& final_slices = solution.pizza().slices();

  int total_cells = 0;
  std::string result_string;
  for (const auto& slice : final_slices) {
    result_string += slice.ToString() + "\n";
    total_cells += slice.area();
  }
  output << total_cells << std::endl;
  output << result_string;

  std::cout << "Total cells: " << total_cells
            << ", slices num = " << final_slices.size() << std::endl;

  VerifySolution(initial_cells, final_slices, l, h, r, c);

  return 0;
}

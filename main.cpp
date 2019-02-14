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
                    const vector<Slice> slices, int l, int h) {
  for (const auto& slice : slices) {
    if (slice.area() > h) {
      std::cout << "Verification error: too big. " << slice.ToString()
                << std::endl;
    }

    int tomato = 0;
    int marshrooms = 0;
    for (int i = slice.r1; i <= slice.r2; ++i) {
      for (int j = slice.c1; j <= slice.c2; ++j) {
        if (initial_cells[i][j].slice_id) {
          std::cout << "Verification error: intersection found "
                    << slice.ToString() << std::endl;
        } else {
          initial_cells[i][j].slice_id = 1;
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
  if (argc < 2) {
    std::cout << "Format rror: <input_file> --[greedy|graph]" << std::endl;
    return -1;
  }

  std::ifstream input(argv[1]);
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

  vector<Slice> final_slices;
  if (argc == 3 && argv[2] == std::string("--graph")) {
    // graph::Solution solution(Pizza(initial_cells, l, h));
    // solution.solve();
  } else {
    greedy::Solution solution(Pizza(initial_cells, l, h));
    solution.solve();
    final_slices = solution.pizza().slices();
  }

  int total_cells = 0;
  for (const auto& slice : final_slices) {
    //  std:cout << slice.ToString() << std::endl;
    total_cells += slice.area();
  }
  std::cout << "Total cells: " << total_cells
            << ", slices num = " << final_slices.size() << std::endl;

  VerifySolution(initial_cells, final_slices, l, h);

  return 0;
}

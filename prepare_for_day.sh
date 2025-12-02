#!/bin/bash

# Check for correct number of arguments
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <day as two digit val>"
  exit 1
fi

template_file_h="./src/include/aoc2025_dd.hpp"
template_file_cpp="./src/cpp/aoc2025_dd.cpp"
replacement="$1"

./replace_dd.sh "$template_file_h" "$replacement"
./replace_dd.sh "$template_file_cpp" "$replacement"

# adjust shortcut in vim.rc
sed -i -E "s/-day [0-9]+/-day $replacement/; s/-part [0-9]+/-part 1/" vim.rc


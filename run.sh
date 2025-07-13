#!/usr/bin/fish
cmake -S . -B build -G Ninja
cd build; ninja -j14; ./main

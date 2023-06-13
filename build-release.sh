#!/bin/zsh
mkdir build/
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S ./ -B build/
ninja -j 16 -C build/
./build/parksnrec

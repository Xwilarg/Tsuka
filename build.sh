#!/bin/sh
set -e
mkdir -p build                   
cd build
cmake ..
cmake --build . --config RELEASE
cp Release/Tsuka.exe ..
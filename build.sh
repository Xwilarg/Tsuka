#!/bin/sh
set -e
mkdir -p build
cd build
cmake ..
cmake --build . --config RELEASE
if [ -d "Release" ]; then # MSVC
    cp Release/Tsuka.exe ..
else
    cp Tsuka* ..
fi
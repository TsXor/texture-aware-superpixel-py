#!/bin/bash
cd $(dirname $0)
mkdir build
cd build
cmake ..
cmake --build . --config release
cd ..
cp -f ./build/Release/TSSP.* .
rm -rf build >/dev/null
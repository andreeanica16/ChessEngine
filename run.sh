#!/bin/bash

cmake .
make
xboard -debug -fcp "./ladybugs"
rm CMakeCache.txt
rm -r CMakeFiles
rm cmake_install.cmake
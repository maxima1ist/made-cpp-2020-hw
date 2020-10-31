#!/bin/bash

set -e

g++ -std=c++17 -I./src test/test.cpp -o geometry
./geometry

echo All tests passed!
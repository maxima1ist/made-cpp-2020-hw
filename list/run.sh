#!/bin/bash

set -e

g++ -std=c++17 -I./ test/test.cpp -o list_test
./list_test

echo All tests passed!
#!/bin/bash

set -e

g++ -std=c++17 -I./ test/test.cpp -o smart_pointers_test
./smart_pointers_test

echo All tests passed!
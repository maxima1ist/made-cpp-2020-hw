#!/bin/bash

set -e

STRESS_TEST_COUNT=500

g++ -std=c++17 -I./ test/test.cpp src/matrix.cpp -o matrix_test
python3 test/generate.py $STRESS_TEST_COUNT > test_data
./matrix_test $STRESS_TEST_COUNT < test_data

rm test_data

echo All tests passed!

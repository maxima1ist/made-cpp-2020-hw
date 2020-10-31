#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <sstream>
#include <cmath>
#include "src/matrix.h"


using task::Matrix;


size_t RandomUInt(size_t max = -1) {
    static std::mt19937 rand(std::random_device{}());

    std::uniform_int_distribution<size_t> dist{0, max};
    return dist(rand);
}

size_t RandomUInt(size_t min, size_t max) {
    return min + RandomUInt(max - min);
}

bool TossCoin() {
    return RandomUInt(1) == 0;
}


double RandomDouble() {
    static std::mt19937 rand(std::random_device{}());

    std::uniform_real_distribution<double> dist{-10., 10.};
    return dist(rand);
}

Matrix RandomMatrix(size_t rows, size_t cols) {
    Matrix temp(rows, cols);
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            temp[row][col] = RandomDouble();
        }
    }
    return temp;
}


void FailWithMsg(const std::string& msg, int line) {
    std::cerr << "Test failed!\n";
    std::cerr << "[Line " << line << "] "  << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

#define ASSERT_TRUE(cond) \
    if (!(cond)) {FailWithMsg("Assertion failed: " #cond, __LINE__);};

#define ASSERT_TRUE_MSG(cond, msg) \
    if (!(cond)) {FailWithMsg(msg, __LINE__);};

#define ASSERT_EXCEPTION_MSG(cond, ex, msg) \
    {bool ok = false;                       \
    try {(cond);} catch (const ex&) {ok = true;} catch (...) {} \
    if (!ok) FailWithMsg(msg, __LINE__);}


#define REPEAT(count) for (size_t _iter = 0; _iter < (count); ++_iter)


const double EPS = 1e-6;


int main(int argc, char** argv) {

    {
        Matrix mat;
        ASSERT_TRUE_MSG(mat[0][0] == 1., "Default constructor")

        Matrix mat1(2, 2);

        mat1 = mat1;

        mat1[0][1] = 300.;
        mat1[1][0] = 200.;
        ASSERT_TRUE_MSG(mat1[0][0] == 1., "Rows / cols constructor")
        ASSERT_TRUE_MSG(mat1[1][1] == 1., "Rows / cols constructor")
        ASSERT_TRUE_MSG(mat1[0][1] == 300., "Operator []")
        ASSERT_TRUE_MSG(mat1[1][0] == 200., "Operator []")

        auto partial = mat1[1];
        partial[1] = 400.;

        ASSERT_TRUE_MSG(mat1[1][1] == 400., "Operator []")

        const Matrix mat_c = mat1;
        auto partial_c = mat_c[1];

        ASSERT_TRUE_MSG(partial_c[1] == 400., "Operator []")


        ASSERT_TRUE_MSG(mat1.get(1, 1) == 400., "get()")
        ASSERT_TRUE_MSG(mat_c.get(1, 1) == 400., "get()")

        auto mat2 = mat1;

        mat2.set(0, 1, 100.);
        ASSERT_TRUE(mat1.get(0, 1) == 300.)
        ASSERT_TRUE(mat2.get(0, 1) == 100.)

        ASSERT_EXCEPTION_MSG(mat.get(1, 0), task::OutOfBoundsException, "get()")
        ASSERT_EXCEPTION_MSG(mat.set(0, 1, 10.), task::OutOfBoundsException, "set()")

        ASSERT_EXCEPTION_MSG(mat_c.get(2, 0), task::OutOfBoundsException, "get()")
        ASSERT_EXCEPTION_MSG(mat2.set(0, 2, 10.), std::exception, "set()")

        mat.resize(2, 2);

        ASSERT_EXCEPTION_MSG(mat.get(2, 0), task::OutOfBoundsException, "resize()")
        ASSERT_EXCEPTION_MSG(mat.get(0, 2), task::OutOfBoundsException, "resize()")

        ASSERT_TRUE_MSG(mat[0][0] == 1. && mat[0][1] == 0. && mat[1][0] == 0. && mat[1][1] == 0., "resize()")

        /*
        REPEAT(1000) {
            // oh boy i sure can't wait to resize
            mat.resize(1000, 1000);
            mat.resize(500, 500);
            // what do you mean memory leaks?
        }
        */

        auto mat3 = RandomMatrix(1000, 1000);
        for (size_t i = 0; i < 1000; ++i) {
            auto row = mat3.getRow(i);
            auto column = mat3.getColumn(i);
            for (size_t j = 0; j < 1000; ++j) {
                ASSERT_TRUE_MSG(row[j] = mat3.get(i, j), "getRow()")
                ASSERT_TRUE_MSG(column[j] = mat3.get(j, i), "getColumn()")
            }
        }

    }

    REPEAT(10)
    {
        size_t n = RandomUInt(1, 200);
        size_t m = RandomUInt(1, 200);
        auto mat1 = RandomMatrix(RandomUInt(1, 200), n);
        auto mat2 = RandomMatrix(n + RandomUInt(1, 20), RandomUInt(1, 200));
        ASSERT_EXCEPTION_MSG(mat1 * mat2, task::SizeMismatchException, "Exceptions");
        ASSERT_EXCEPTION_MSG(mat1 *= mat2, task::SizeMismatchException, "Exceptions");


        mat1 = RandomMatrix(n, m);
        if (TossCoin()) {
            mat2 = RandomMatrix(n + RandomUInt(1, 200), m);
        } else {
            mat2 = RandomMatrix(n, m + RandomUInt(1, 200));
        }
        ASSERT_EXCEPTION_MSG(mat1 + mat2, task::SizeMismatchException, "Exceptions");
        ASSERT_EXCEPTION_MSG(mat1 - mat2, task::SizeMismatchException, "Exceptions");
        ASSERT_EXCEPTION_MSG(mat1 += mat2, task::SizeMismatchException, "Exceptions");
        ASSERT_EXCEPTION_MSG(mat1 -= mat2, task::SizeMismatchException, "Exceptions");


        if (TossCoin()) {
            mat1 = RandomMatrix(n + RandomUInt(1, 200), n);
        } else {
            mat1 = RandomMatrix(n, n + RandomUInt(1, 200));
        }
        ASSERT_EXCEPTION_MSG(mat1.det(), task::SizeMismatchException, "Exceptions");
        ASSERT_EXCEPTION_MSG(mat1.trace(), task::SizeMismatchException, "Exceptions");
    }

    REPEAT(100)
    {
        auto rows = RandomUInt(1, 100), cols = RandomUInt(1, 100);
        auto mat1 = RandomMatrix(rows, cols);
        auto mat2 = mat1;

        ASSERT_TRUE_MSG(mat1 == mat2, "Operator ==")
        ASSERT_TRUE_MSG(!(mat1 != mat2), "Operator !=")

        mat2[RandomUInt(0, rows - 1)][RandomUInt(0, cols - 1)] += TossCoin() ? EPS / 2 : -EPS / 2;

        ASSERT_TRUE_MSG(mat1 == mat2, "Operator ==")
        ASSERT_TRUE_MSG(!(mat1 != mat2), "Operator !=")

        mat2[RandomUInt(0, rows - 1)][RandomUInt(0, cols - 1)] += TossCoin() ? EPS * 2 : -EPS * 2;

        ASSERT_TRUE_MSG(!(mat1 == mat2), "Operator ==")
        ASSERT_TRUE_MSG(mat1 != mat2, "Operator !=")
    }


    REPEAT(100)
    {
        auto mat1 = RandomMatrix(100, 50);
        auto mat2 = RandomMatrix(1, 2);

        std::stringstream stream;
        stream.precision(10);
        stream << "100 50" << '\n';
        stream << mat1;

        ASSERT_TRUE_MSG(*(stream.str().end() - 1) == '\n', "Stream output operator")

        stream >> mat2;
        ASSERT_TRUE_MSG(mat1 == mat2, "Stream input / output operator")
    }


    const int STRESS_TEST_COUNT = argc > 1 ? std::stoi(argv[1]) : 0;

    REPEAT(STRESS_TEST_COUNT)
    {
        Matrix mat1, mat2, ans, res;
        double scalar;

        std::cin >> mat1 >> mat2 >> ans;

        if (_iter % 2 == 0) {
            res = mat1;
            res += mat2;
        } else {
            res = mat1 + mat2;
        }

        ASSERT_TRUE_MSG(res == ans, "Operator + / +=")


        std::cin >> ans;

        if (_iter % 2 == 0) {
            res = mat1;
            res -= mat2;
        } else {
            res = mat1 - mat2;
        }

        ASSERT_TRUE_MSG(res == ans, "Operator - / -=")


        std::cin >> mat2 >> ans;

        if (_iter % 2 == 0) {
            res = mat1;
            res *= mat2;
        } else {
            res = mat1 * mat2;
        }

        ASSERT_TRUE_MSG(res == ans, "Matrix operator * / *=")


        std::cin >> scalar >> ans;

        if (_iter % 3 == 0) {
            res = mat1;
            res *= scalar;
        } else if (_iter % 3 == 1){
            res = mat1 * scalar;
        } else {
            res = scalar * mat1;
        }

        ASSERT_TRUE_MSG(res == ans, "Scalar operator * / *=")


        std::cin >> ans;
        ASSERT_TRUE_MSG(-mat1 == ans, "Unary -")
        ASSERT_TRUE_MSG(+mat1 == mat1, "Unary +")


        std::cin >> ans;
        if (_iter % 2 == 0) {
            res = mat1;
            res.transpose();
        } else {
            res = mat1.transposed();
        }
        ASSERT_TRUE_MSG(res == ans, "Transpose")


        std::cin >> mat1 >> scalar;
        ASSERT_TRUE_MSG(fabs(mat1.trace() - scalar) < EPS, "Trace")


        std::cin >> mat2 >> scalar;
        ASSERT_TRUE_MSG(fabs(mat2.det() - scalar) < EPS * 10., "Determinant")
    }

}

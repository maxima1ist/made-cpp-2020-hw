#include "matrix.h"
#include <iostream>

namespace task {

    Matrix::MatrixRow::MatrixRow(double* row, const size_t& cols) : row(row), cols(cols) {}

    double& Matrix::MatrixRow::operator[](const size_t& col) {
        if (col >= cols)
            throw OutOfBoundsException();
        return row[col];
    }

    const double& Matrix::MatrixRow::operator[](const size_t& col) const {
        if (col >= cols)
            throw OutOfBoundsException();
        return row[col];
    }

    Matrix::Matrix() : matrix(new double* [1]), rows(1), cols(1) {
        matrix[0] = new double[1];
        matrix[0][0] = 1;
    }

    Matrix::Matrix(const size_t& rows, const size_t& cols) : matrix(new double* [rows]), rows(rows), cols(cols) {
        for (size_t i = 0; i < rows; ++i) {
            matrix[i] = new double[cols];
            for (size_t j = 0; j < cols; ++j) {
                if (i == j)
                    matrix[i][j] = 1;
                else
                    matrix[i][j] = 0;
            }
        }
    }

    Matrix::Matrix(const Matrix& copy) : matrix(new double* [copy.rows]), rows(copy.rows), cols(copy.cols) {
        for (size_t i = 0; i < rows; ++i) {
            matrix[i] = new double[cols];
            for (size_t j = 0; j < cols; ++j)
                matrix[i][j] = copy.matrix[i][j];
        }
    }

    Matrix& Matrix::operator=(const Matrix& copy) {
        if (&copy == this)
            return *this;
        destroy();
        rows = copy.rows;
        cols = copy.cols;
        matrix = new double* [rows];
        for (size_t i = 0; i < rows; ++i) {
            matrix[i] = new double[cols];
            for (size_t j = 0; j < cols; ++j)
                matrix[i][j] = copy.matrix[i][j];
        }
        return *this;
    }

    Matrix::~Matrix() {
        destroy();
    }

    void Matrix::destroy() {
        if (rows > 0) {
            if (cols > 0)
                for (size_t i = 0; i < rows; ++i)
                    delete[]matrix[i];
            delete[]matrix;
        }
    }

    double Matrix::abs(const double& value) const {
        return value < 0 ? -value : value;
    }

    size_t Matrix::getRowsCount() const {
        return rows;
    }

    size_t Matrix::getColumnsCount() const {
        return cols;
    }

    double& Matrix::get(const size_t& row, const size_t& col) {
        if (row >= rows || col >= cols)
            throw OutOfBoundsException();
        return matrix[row][col];
    }

    const double& Matrix::get(const size_t& row, const size_t& col) const {
        if (row >= rows || col >= cols)
            throw OutOfBoundsException();
        return matrix[row][col];
    }

    void Matrix::set(const size_t& row, const size_t& col, const double& value) {
        if (row >= rows || col >= cols)
            throw OutOfBoundsException();
        matrix[row][col] = value;
    }

    void Matrix::resize(const size_t& newRows, const size_t& newCols) {
        double** newMatrix = new double* [newRows];
        for (size_t i = 0; i < newRows; ++i) {
            newMatrix[i] = new double[newCols];
            for (size_t j = 0; j < newCols; ++j) {
                if (i < rows && j < cols)
                    newMatrix[i][j] = matrix[i][j];
                else
                    newMatrix[i][j] = 0;
            }
        }
        destroy();
        rows = newRows;
        cols = newCols;
        matrix = newMatrix;
    }

    Matrix::MatrixRow Matrix::operator[](const size_t& row) {
        if (row >= rows)
            throw OutOfBoundsException();
        MatrixRow buffer(matrix[row], cols);
        return buffer;
    }

    const Matrix::MatrixRow Matrix::operator[](const size_t& row) const {
        if (row >= rows)
            throw OutOfBoundsException();
        MatrixRow buffer(matrix[row], cols);
        return buffer;
    }

    Matrix& Matrix::operator+=(const Matrix& add) {
        if (add.rows != rows || add.cols != cols)
            throw SizeMismatchException();
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                matrix[i][j] += add.matrix[i][j];
        return *this;
    }

    Matrix& Matrix::operator-=(const Matrix& diff) {
        if (diff.rows != rows || diff.cols != cols)
            throw SizeMismatchException();
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                matrix[i][j] -= diff.matrix[i][j];
        return *this;
    }

    Matrix& Matrix::operator*=(const Matrix& mult) {
        if (mult.rows != cols)
            throw SizeMismatchException();
        Matrix buffer(rows, mult.cols);
        for (size_t i = 0; i < buffer.rows; ++i)
            for (size_t j = 0; j < buffer.cols; ++j) {
                if (i == j)
                    --buffer.matrix[i][j];
                for (size_t k = 0; k < cols; ++k)
                    buffer.matrix[i][j] += matrix[i][k] * mult.matrix[k][j];
            }
        *this = buffer;
        return *this;
    }

    Matrix& Matrix::operator*=(const double& number) {
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                matrix[i][j] *= number;
        return *this;
    }

    Matrix Matrix::operator+(const Matrix& add) const {
        if (add.rows != rows || add.cols != cols)
            throw SizeMismatchException();
        Matrix buffer(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                buffer.matrix[i][j] = matrix[i][j] + add.matrix[i][j];
        return buffer;
    }

    Matrix Matrix::operator-(const Matrix& diff) const {
        if (diff.rows != rows || diff.cols != cols)
            throw SizeMismatchException();
        Matrix buffer(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                buffer.matrix[i][j] = matrix[i][j] - diff.matrix[i][j];
        return buffer;
    }

    Matrix Matrix::operator*(const Matrix& mult) const {
        if (mult.rows != cols)
            throw SizeMismatchException();
        Matrix buffer(rows, mult.cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < mult.cols; ++j) {
                if (i == j)
                    --buffer.matrix[i][j];
                for (size_t k = 0; k < cols; ++k)
                    buffer.matrix[i][j] += matrix[i][k] * mult.matrix[k][j];
            }
        return buffer;
    }

    Matrix Matrix::operator*(const double& number) const {
        Matrix buffer(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                buffer.matrix[i][j] = matrix[i][j] * number;
        return buffer;
    }

    Matrix Matrix::operator-() const {
        Matrix buffer(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                buffer.matrix[i][j] = -matrix[i][j];
        return buffer;
    }

    Matrix Matrix::operator+() const {
        Matrix buffer(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                buffer.matrix[i][j] = matrix[i][j];
        return buffer;
    }

    double Matrix::det() const {
        if (rows != cols || rows == 0)
            throw SizeMismatchException();
        return det(rows, matrix);
    }

    double Matrix::det(const size_t& size, double** matrixPart) const {
        if (size == 1)
            return matrixPart[0][0];
        double result = 0;
        for (size_t i = 0; i < size; ++i) {
            double** newMatrix = new double* [size - 1];
            for (size_t j = 0; j < size - 1; ++j) {
                newMatrix[j] = new double[size - 1];
                for (size_t k = 0; k < size - 1; ++k) {
                    if (j < i)
                        newMatrix[j][k] = matrixPart[j][k + 1];
                    else
                        newMatrix[j][k] = matrixPart[j + 1][k + 1];
                }
            }
            double add = matrixPart[i][0] * det(size - 1, newMatrix);
            result += (i % 2 == 0 ? add : -add);
            for (size_t j = 0; j < size - 1; ++j)
                delete[] newMatrix[j];
            delete[] newMatrix;
        }
        return result;
    }

    void Matrix::transpose() {
        if (rows == cols) {
            for (size_t i = 1; i < rows; ++i)
                for (size_t j = 0; j <= i; ++j)
                    std::swap(matrix[i][j], matrix[j][i]);
        }
        else {
            double** buffer = new double* [cols];
            for (size_t i = 0; i < cols; ++i) {
                buffer[i] = new double[rows];
                for (size_t j = 0; j < rows; ++j)
                    buffer[i][j] = matrix[j][i];
            }
            destroy();
            std::swap(rows, cols);
            matrix = buffer;
        }
    }

    Matrix Matrix::transposed() const {
        double** buffer = new double* [cols];
        for (size_t i = 0; i < cols; ++i) {
            buffer[i] = new double[rows];
            for (size_t j = 0; j < rows; ++j)
                buffer[i][j] = matrix[j][i];
        }
        Matrix result;
        result.rows = cols;
        result.cols = rows;
        result.matrix = buffer;
        return result;
    }

    double Matrix::trace() const {
        if (rows != cols || rows == 0)
            throw SizeMismatchException();
        double result = 0;
        for (size_t i = 0; i < cols; ++i)
            for (size_t j = 0; j < rows; ++j)
                if (i == j)
                    result += matrix[j][i];
        return result;
    }

    std::vector<double> Matrix::getRow(const size_t& row) {
        std::vector<double> arr;
        if (row >= rows)
            throw OutOfBoundsException();
        for (size_t i = 0; i < cols; ++i)
            arr.push_back(matrix[row][i]);
        return arr;
    }

    std::vector<double> Matrix::getColumn(const size_t& col) {
        std::vector<double> arr;
        if (col >= cols)
            throw OutOfBoundsException();
        for (size_t i = 0; i < cols; ++i)
            arr.push_back(matrix[i][col]);
        return arr;
    }

    bool Matrix::operator==(const Matrix& comp) const {
        if (rows != comp.rows || cols != comp.cols)
            return false;
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                if (abs(matrix[i][j] - comp.matrix[i][j]) > EPS)
                    return false;
        return true;
    }

    bool Matrix::operator!=(const Matrix& comp) const {
        return !(*this == comp);
    }

    Matrix operator*(const double& number, const Matrix& mult) {
        return mult * number;
    }

    std::ostream& operator<<(std::ostream& out, const Matrix& matrix) {
        for (size_t i = 0; i < matrix.getRowsCount(); ++i) {
            for (size_t j = 0; j < matrix.getColumnsCount(); ++j)
                out << matrix[i][j] << ' ';
            out << '\n';
        }
        return out;
    }

    std::istream& operator>>(std::istream& in, Matrix& matrix) {
        size_t rows, cols;
        in >> rows >> cols;
        matrix.resize(rows, cols);
        for (size_t i = 0; i < matrix.getRowsCount(); ++i)
            for (size_t j = 0; j < matrix.getColumnsCount(); ++j)
                in >> matrix[i][j];
        return in;
    }

}   // namespace std
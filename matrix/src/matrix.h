#pragma once

#include <vector>
#include <iostream>

namespace task {

    const double EPS = 1e-6;

    class OutOfBoundsException : public std::exception {};
    class SizeMismatchException : public std::exception {};

    class Matrix {
    private:

        class MatrixRow {
        private:
            double* row;
            size_t cols;

        public:
            explicit MatrixRow(double*, const size_t&);

            double& operator[](const size_t&);
            const double& operator[](const size_t&) const;

        };

        double** matrix;
        size_t rows, cols;

        void destroy();
        double abs(const double&) const;
        double det(const size_t&, double**) const;

    public:

        Matrix();
        explicit Matrix(const size_t&, const size_t&);
        Matrix(const Matrix&);

        ~Matrix();

        Matrix& operator=(const Matrix&);

        size_t getRowsCount() const;
        size_t getColumnsCount() const;

        double& get(const size_t&, const size_t&);
        const double& get(const size_t&, const size_t&) const;
        void set(const size_t&, const size_t&, const double&);
        void resize(const size_t&, const size_t&);

        MatrixRow operator[](const size_t&);
        const MatrixRow operator[](const size_t&) const;

        Matrix& operator+=(const Matrix&);
        Matrix& operator-=(const Matrix&);
        Matrix& operator*=(const Matrix&);
        Matrix& operator*=(const double&);

        Matrix operator+(const Matrix&) const;
        Matrix operator-(const Matrix&) const;
        Matrix operator*(const Matrix&) const;
        Matrix operator*(const double&) const;

        Matrix operator-() const;
        Matrix operator+() const;

        double det() const;
        void transpose();
        Matrix transposed() const;
        double trace() const;

        std::vector<double> getRow(const size_t&);
        std::vector<double> getColumn(const size_t&);

        bool operator==(const Matrix&) const;
        bool operator!=(const Matrix&) const;

    };

    Matrix operator*(const double&, const Matrix&);

    std::ostream& operator<<(std::ostream&, const Matrix&);
    std::istream& operator>>(std::istream&, Matrix&);

}  // namespace task

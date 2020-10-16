#pragma once
#include <vector>
#include <iostream>

extern const double EPS;

namespace task {

    std::vector<double> operator+(const std::vector<double>& arr) {
        std::vector<double> result(arr.size());
        for (size_t i = 0; i < arr.size(); ++i) {
            result[i] = arr[i];
        }

        return result;
    }

    std::vector<double> operator+(const std::vector<double>& arrOne, const std::vector<double>& arrTwo) {
        if (arrOne.size() != arrTwo.size()) {
            throw "Vectors must have the same size!";
        }

        std::vector<double> result(arrOne.size());
        for (size_t i = 0; i < arrOne.size(); ++i) {
            result[i] = arrOne[i] + arrTwo[i];
        }

        return result;
    }

    std::vector<double> operator-(const std::vector<double>& arr) {
        std::vector<double> result(arr.size());
        for (size_t i = 0; i < arr.size(); ++i) {
            result[i] = -arr[i];
        }

        return result;
    }

    std::vector<double> operator-(const std::vector<double>& arrOne, const std::vector<double>& arrTwo) {
        if (arrOne.size() != arrTwo.size()) {
            throw "Vectors must have the same size!";
        }

        std::vector<double> result(arrOne.size());
        for (size_t i = 0; i < arrOne.size(); ++i) {
            result[i] = arrOne[i] - arrTwo[i];
        }

        return result;
    }

    double operator*(const std::vector<double>& arrOne, const std::vector<double>& arrTwo) {
        if (arrOne.size() != arrTwo.size()) {
            throw "Vectors must have the same size!";
        }

        double result = 0;
        for (size_t i = 0; i < arrOne.size(); ++i) {
            result += arrOne[i] * arrTwo[i];
        }

        return result;
    }

    std::vector<double> operator%(const std::vector<double>& arrOne, const std::vector<double>& arrTwo) {
        if (arrOne.size() != arrTwo.size()) {
            throw "Vectors must have the same size!";
        }

        if (arrOne.size() == 3) {
            std::vector<double> result(arrOne.size());
            for (size_t i = 0; i < arrOne.size(); ++i) {
                if (i != 0 && i != arrOne.size() - 1) {
                    result[i] = arrOne[i - 1] * arrTwo[i + 1] - arrOne[i + 1] * arrTwo[i - 1];
                } else {
                    if (i == 0) {
                        result[i] = arrOne[i + 1] * arrTwo[i + 2] - arrOne[i + 2] * arrTwo[i + 1];
                    } else {
                        result[i] = arrOne[i - 2] * arrTwo[i - 1] - arrOne[i - 1] * arrTwo[i - 2];
                    }
                }
                result[i] *= pow(-1, i % 2);
            }

            return result;
        } else {
            throw "Vectors size must be 3!";
        }
    }

    bool isNullVector(const std::vector<double>& arr) {
        for (double it : arr) {
            if (it != 0) {
                return false;
            }
        }

        return true;
    }

    bool isCollinear(const std::vector<double>& arrOne, const std::vector<double>& arrTwo, double& k) {
        if (arrOne.size() != arrTwo.size()) {
            throw "Vectors must have the same size!";
        }
        if (arrOne.size() == 0) {
            throw "Vector size must not be null size!";
        }
        if (isNullVector(arrOne) || isNullVector(arrTwo)) {
            throw "Both of vectors must not be null vector!";
        }

        bool wasK = false;
        for (size_t i = 0; i < arrOne.size(); ++i) {
            if (arrOne[i] == 0) {
                if (arrTwo[i] != 0) {
                    return false;
                }
            } else {
                if (wasK) {
                    if (std::abs(k - arrTwo[i] / arrOne[i]) > EPS) {
                        return false;
                    }
                } else {
                    k = arrTwo[i] / arrOne[i];
                    wasK = true;
                }
            }
        }

        return true;
    }

    bool operator||(const std::vector<double>& arrOne, const std::vector<double>& arrTwo) {
        double* k = new double;
        if (isCollinear(arrOne, arrTwo, *k)) {
            delete k;
            return true;
        } else {
            delete k;
            return false;
        }
    }

    bool operator&&(const std::vector<double>& arrOne, const std::vector<double>& arrTwo) {
        double* k = new double;
        if (isCollinear(arrOne, arrTwo, *k) && *k > 0) {
            delete k;
            return true;
        } else {
            delete k;
            return false;
        }
    }

    std::istream& operator>>(std::istream& in, std::vector<double>& arr) {
        size_t size;
        in >> size;
        if (size < 0) {
            throw "Vector size must not be negative!";
        }

        arr.resize(size);
        for (size_t i = 0; i < size; ++i) {
            in >> arr[i];
        }

        return in;
    }

    std::ostream& operator<<(std::ostream& out, const std::vector<double>& arr) {
        for (auto it : arr) {
            out << it << ' ';
        }
        out << '\n';

        return out;
    }

    std::vector<double> reverse(std::vector<double>& arr) {
        for (size_t i = 0; i < arr.size() / 2; ++i) {
            std::swap(arr[i], arr[arr.size() - 1 - i]);
        }

        return arr;
    }

    std::vector<int> operator|(const std::vector<int>& arrOne, const std::vector<int>& arrTwo) {
        if (arrOne.size() != arrTwo.size()) {
            throw "Vectors must have the same size!";
        }

        std::vector<int> result(arrOne.size());
        for (size_t i = 0; i < arrOne.size(); ++i) {
            result[i] = arrOne[i] | arrTwo[i];
        }

        return result;
    }

    std::vector<int> operator&(const std::vector<int>& arrOne, const std::vector<int>& arrTwo) {
        if (arrOne.size() != arrTwo.size()) {
            throw "Vectors must have the same size!";
        }

        std::vector<int> result(arrOne.size());
        for (size_t i = 0; i < arrOne.size(); ++i) {
            result[i] = arrOne[i] & arrTwo[i];
        }

        return result;
    }

}

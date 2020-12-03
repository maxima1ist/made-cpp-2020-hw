#include <iostream>
#include <functional>
#include "function.h"
#include "compose.h"

using std::cout;
using std::endl;

using task::function;

typedef function<void()> EmptyFunction;
typedef function<int(const int*, int)> Function;

struct SumOfTwo {
    int operator()(const int* a, int b) const {
        return *a + b;
    }
};

int sumOfTwo(const int* a, int b) {
    return *a + b;
}

int main() {
    {
        cout << "function" << endl << endl;

        int x = 10;

        //test 0
        EmptyFunction f0;
        try {
            cout << "test 0: ";
            f0();
        } catch (const char* exp) {
            cout << exp << endl << "correct" << endl;
        }
        cout << endl;

        //test 01
        EmptyFunction f01(nullptr);
        try {
            cout << "test 01: ";
            f01();
        } catch (const char* exp) {
            cout << exp << endl << "correct" << endl;
        }
        cout << endl;

        //test 1
        Function f1(sumOfTwo);
        int answer = f1(&x, 1);
        std::cout << "test 1: " << answer << std::endl;
        if (answer == 11) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        //test 11
        Function f11((SumOfTwo()));
        answer = f11(&x, 11);
        std::cout << "test 11: " << answer << std::endl;
        if (answer == 21) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        //test 2
        Function f2(f1);
        answer = f2(&x, 2);
        std::cout << "test 2: " << answer << std::endl;
        if (answer == 12) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        //test 3
        Function f3(std::forward<Function>(f2));
        answer = f3(&x, 3);
        try {
            std::cout << "test 3: " << f3(&x, 3) << ", is move testing: " << f2(&x, 100);
        } catch (const char* exp) {
            cout << exp << endl;
            if (answer == 13) {
                cout << "correct" << endl;
            } else {
                cout << "failed" << endl;
                return 0;
            }
        }
        cout << endl;

        //test 4
        Function f4 = f3;
        answer = f4(&x, 44);
        int answer1 = f3(&x, 43);
        std::cout << "test 4: " << f4(&x, 44) << ", " << f3(&x, 43) << endl;
        if (answer == 54 && answer1 == 53) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        //test 41
        Function f41(sumOfTwo);
        f41 = nullptr;
        try {
            cout << "test 41: ";
            f41(&x, 100);
        } catch (const char* exp) {
            cout << exp << endl << "correct" << endl;
        }
        cout << endl;

        //test 5
        f2 = std::forward<Function>(f3);
        try {
            std::cout << "test 5: " << f2(&x, 52) << ", is move testing: " << f3(&x, 53);
        } catch (const char* exp) {
            cout << exp << endl << "correct" << endl;
        }
        cout << endl;

        //test 6
        f2 = [](const int* a, int b) -> int { return *a * b; };
        answer = f2(&x, 6);
        std::cout << "test 6: " << f2(&x, 6) << endl;
        if (answer == 60) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        //test 7
        answer = f2(&x, 7), answer1 = f1(&x, 7);
        cout << "test 7: " << f2(&x, 7) << ", " << f1(&x, 7);
        f2.swap(f1);
        cout << ". Swap: " << f2(&x, 7) << ", " << f1(&x, 7) << endl;
        if (answer1 == f2(&x, 7) && answer == f1(&x, 7)) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }

        std::swap(f1, f2);
        cout << "Again swap: " << f2(&x, 7) << ", " << f1(&x, 7) << endl;
        if (answer == f2(&x, 7) && answer1 == f1(&x, 7)) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        //test 8
        cout << "test 8:" << endl;
        cout << "f2 is " << (f2 ? "not empty" : "empty") << endl;
        cout << "f3 is " << (f3 ? "not empty" : "empty") << endl;
        if (f2 && !f3) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        //test 9
        cout << "test 9: " << endl;
        cout << f2.target_type().name() << endl;
        cout << f3.target_type().name() << endl;
        cout << "correct" << endl;
        cout << endl;

        //test 10
        cout << "test 10: " << endl;
        f2 = sumOfTwo;
        int (*const* ptr)(const int*, int) = f2.target < int(*)(const int*, int)>();
        if (ptr && *ptr == sumOfTwo) {
            cout << "correct" << endl;
        } else {
            cout << "failed" << endl;
            return 0;
        }
        cout << endl;

        cout << "function: all tests passed!" << endl;
    }

    cout << endl << endl;

    {
        cout << "compose" << endl << endl;

        const std::function<bool(int)> a = [](int x) -> bool {
            std::cout << "a" << std::endl;
            return x > 10;
        };

        const std::function<int(float)> b = [](float y) -> int {
            std::cout << "b" << std::endl;
            return int(y * y);
        };

        const std::function<float(bool)> c = [](bool z) -> float {
            std::cout << "c" << std::endl;
            return z ? 3.1f : 3.34f;
        };

        const std::function<int(const std::string&, int, bool)> d = [](const std::string&, int, bool) -> int {
            std::cout << "d" << std::endl;
            return 0;
        };

        //test 1
        cout << "test 1:" << endl;
        auto f1 = compose(a, b, c, a, b, c);
        f1(true);
        cout << "correct" << endl;
        cout << endl;

        //test 2
        cout << "test 2:" << endl;
        auto f2 = compose(d);
        f2("he", 1, 1);
        cout << "correct" << endl;
        cout << endl;

        //test 3
        cout << "test 3:" << endl;
        auto f3 = compose(a);
        f3(12);
        cout << "correct" << endl;
        cout << endl;

        //Attention! Discommenting this code will cause compilation error named "types error!"
        //test X
        /*
        auto f4 = compose(a, a);
        f4(12);
        */

        //Attention! Discommenting this code will cause compilation error named "types error!"
        //test XX
        /*
        auto f5 = compose(a, c, b);
        f5(12);
        */

        cout << "compose: all tests passed!" << endl;

    }

    return 0;
}
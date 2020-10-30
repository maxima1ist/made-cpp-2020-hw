#include <iostream>
#include <functional>

typedef std::function<int (int)> Op;

int someIdempotentFunction (int x) {
    return x;
}

Op composeOfTwo (Op funcOne, Op funcTwo) {
    return [funcOne, funcTwo] (int x) -> int {
        return funcTwo(funcOne(x));
    };
}

Op compose (size_t n, Op ops[]) {
    if(n <= 0)
        return someIdempotentFunction;

    if(n == 1)
        return ops[0];


    Op result = composeOfTwo(ops[n - 1], ops[n - 2]);
    for(size_t i = 3; i <= n; ++i)
        result = composeOfTwo(result, ops[n - i]);

    return result;
}


int main () {
    /// Simple tests:

    Op op1 =
        [] (int x) -> int {
            return x + 1;
        };

    auto op2 =
        [] (int p) -> Op {
            return [p] (int x) {
                return p * x;
            };
        };

    {
        Op ops[4] = {op1, op2(2), op1, op2(2)};
        if (compose(4, ops)(2) != 11) {
            std::cout << "FAILED AT TEST 1" << std::endl;
            return 0;
        }
    }

    {
        Op ops[8] = {op1, op2(2), op1, op1, op1, op2(2), op2(2), op1};
        if (compose(8, ops)(5) != 55) {
            std::cout << "FAILED AT TEST 2" << std::endl;
            return 0;
        }
    }

    {
        Op ops[1] = {op1};
        if (compose(1, ops)(3) != 4) {
            std::cout << "FAILED AT TEST 3" << std::endl;
            return 0;
        }
    }

    {
        Op ops[0] = {};
        if (compose(0, ops)(4) != 4) {
            std::cout << "FAILED AT TEST 4" << std::endl;
            return 0;
        }
    }

    {
        Op ops[4] = {op2(2), op2(3), op2(4), op2(5)};
        if (compose(4, ops)(1) != 120) {
            std::cout << "FAILED AT TEST 5" << std::endl;
            return 0;
        }
    }
}

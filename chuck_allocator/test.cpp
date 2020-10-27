#include <iostream>
#include <vector>
#include <ostream>
#include "allocator.h"

class A {
private:
    int x, y;

public:
    A(const int& x, const int& y) : x(x), y(y) {
        std::cout << "A(int, int)" << '\n';
    }

    ~A() {
        std::cout << "~A()" << '\n';
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }
};

std::ostream& operator<<(std::ostream& out, const A& a) {
    out << a.getX() << ' ' << a.getY() << '\n';
    return out;
}

int main() {

    Allocator<A> alloc;
    std::vector<A, decltype(alloc)> vec;
    vec.push_back(A(1, 2));
    vec.push_back(A(32, 32));
    vec.push_back(A(42, -1232));
    vec.push_back(A(1012, -2));

    A* arr = alloc.allocate(3);

    alloc.construct(arr, A(1, 2));
    alloc.construct(arr + 1, A(132, 8));
    alloc.construct(arr + 2, A(0, 0));

    for (int i = 0; i < 3; ++i) {
        std::cout << arr[i] << std::endl;
    }

    Allocator<A> allocTwo(alloc);

    Allocator<A> allocThree;
    allocThree = allocTwo;

    allocThree.allocate(2);
    allocThree.construct(arr, A(1, 2));
    allocThree.construct(arr, A(132, 8));

    decltype(allocThree)::rebind<int>::other other;
    int* arrTwo = other.allocate(10);
    other.deallocate(arrTwo, 10);

    return 0;
}
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <vector>
#include "src/smart_pointers.h"

using task::UniquePtr;
using task::SharedPtr;
using task::WeakPtr;


size_t RandomUInt(size_t max = -1) {
    static std::mt19937 rand(std::random_device{}());

    std::uniform_int_distribution<size_t> dist{0, max};
    return dist(rand);
}

size_t RandomUInt(size_t min, size_t max) {
    return min + RandomUInt(max - min);
}


struct Node;

struct Next {
    SharedPtr<Node> shared;
    WeakPtr<Node> weak;
    Next(const SharedPtr<Node>& shared): shared(shared) {}
    Next(const WeakPtr<Node>& weak): weak(weak) {}
};

struct Node {
    int value;
    Next next;
    Node(int value): value(value), next(SharedPtr<Node>()) {}
    Node(int value, const SharedPtr<Node>& next): value(value), next(next) {}
    Node(int value, const WeakPtr<Node>& next): value(value), next(next) {}
    ~Node() {}
};

SharedPtr<Node> getCyclePtr(int cycleSize) {
    SharedPtr<Node> head(new Node(0));
    SharedPtr<Node> prev(head);
    for (int i = 1; i < cycleSize; ++i) {
        SharedPtr<Node> current(new Node(i));
        prev->next.shared = current;
        prev = current;
    }
    prev->next.weak = head;
    return head;
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

#define ASSERT_EQUAL_MSG(cont1, cont2, msg) \
    ASSERT_TRUE_MSG(std::equal(cont1.begin(), cont1.end(), cont2.begin(), cont2.end()), msg)


int main() {

    {
        {
            for (int i = 0; i < 1'000'000; ++i) {
                auto u = UniquePtr<long long>(new long long(5));
                auto uu = UniquePtr<long long>(new long long(10));
                u = std::move(uu);
                ASSERT_TRUE(*u == 10);
                ASSERT_TRUE(uu.get() == nullptr);
                auto uuu = std::move(uu);
                uuu = std::move(u);
            }
        }

        {
            for (int k = 0; k < 10; ++k) {
                std::vector<UniquePtr<int>> v;
                for (int i = 0; i < 100'000; ++i) {
                    v.push_back(UniquePtr<int>(new int(i)));
                }
                std::reverse(v.begin(), v.end());
                ASSERT_TRUE(*v[20'000] == 79'999);
            }
        }

        auto p = UniquePtr<std::string>(new std::string("1234567890"));
        ASSERT_TRUE(p->length() == 10);
    }

    {
        using std::vector;

        auto first_ptr = SharedPtr<vector<int>>(new vector<int>(1'000'000));

        (*first_ptr)[0] = 1;

        vector<int>& vec = *first_ptr;
        auto second_ptr = SharedPtr<vector<int>>(new vector<int>(vec));

        (*second_ptr)[0] = 2;

        for (int i = 0; i < 1'000'000; ++i)
            first_ptr.swap(second_ptr);
        first_ptr->swap(*second_ptr);

        ASSERT_TRUE(first_ptr->front() == 2);
        ASSERT_TRUE(second_ptr->front() == 1);

        ASSERT_TRUE(first_ptr.use_count() == 1);
        ASSERT_TRUE(second_ptr.use_count() == 1);

        for (int i = 0; i < 10; ++i) {
            auto third_ptr = SharedPtr<vector<int>>(new vector<int>(vec));
            auto fourth_ptr = second_ptr;
            fourth_ptr.swap(third_ptr);
            ASSERT_TRUE(second_ptr.use_count() == 2);
        }

        ASSERT_TRUE(second_ptr.use_count() == 1);

        {
            vector<SharedPtr<vector<int>>> ptrs(10, SharedPtr<vector<int>>(first_ptr));
            for (int i = 0; i < 100'000; ++i) {
                ptrs.push_back(ptrs.back());
                ptrs.push_back(SharedPtr<vector<int>>(ptrs.back()));
            }
            ASSERT_TRUE(first_ptr.use_count() == 1 + 10 + 200'000);
        }

        first_ptr.reset(new vector<int>());
        second_ptr.reset();
        SharedPtr<vector<int>>().swap(first_ptr);

        ASSERT_TRUE(second_ptr.get() == nullptr);
        ASSERT_TRUE(second_ptr.get() == nullptr);

        for (int k = 0; k < 2; ++k) {
            vector<SharedPtr<int>> ptrs;
            for (int i = 0; i < 100'000; ++i) {
                int* p = new int(RandomUInt(99'999));
                ptrs.push_back(SharedPtr<int>(p));
            }
            std::sort(ptrs.begin(), ptrs.end(), [](auto&& x, auto&& y){return *x < *y;});
            for (int i = 0; i + 1 < 100'000; ++i) {
                ASSERT_TRUE(*(ptrs[i]) <= *(ptrs[i+1]));
            }
            while (!ptrs.empty()) {
                ptrs.pop_back();
            }
        }
    }

    {
        auto sp = SharedPtr<int>(new int(23));
        WeakPtr<int> weak = sp;
        {
            auto shared = SharedPtr<int>(new int(42));
            weak = shared;
            ASSERT_TRUE(weak.use_count() == 1);
            ASSERT_TRUE(!weak.expired());
        }
        ASSERT_TRUE(weak.use_count() == 0);
        ASSERT_TRUE(weak.expired());

        weak = sp;
        auto wp = weak;
        ASSERT_TRUE(weak.use_count() == 1);
        auto wwp = std::move(weak);
        ASSERT_TRUE(weak.use_count() == 0);
        ASSERT_TRUE(wwp.use_count() == 1);

        auto ssp = wwp.lock();
        ASSERT_TRUE(sp.use_count() == 2);

        for (int i = 0; i < 1'000'000; ++i) {
            SharedPtr<Node> head = getCyclePtr(8);
            SharedPtr<Node> nextHead = head->next.shared;
            ASSERT_TRUE(nextHead.use_count() == 2);
            head.reset();
            ASSERT_TRUE(nextHead.use_count() == 1);
        }
    }

}

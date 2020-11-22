#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <vector>
#include <list>
#include "src/list.h"


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


template <class T>
void RandomFill(T& container, size_t count, size_t max = -1) {
    while (count > 0) {
        container.push_back(RandomUInt(max));
        --count;
    }
}


struct Immovable {
    Immovable() = default;
    Immovable(const Immovable&) = delete;
    Immovable(Immovable&&) = delete;

    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&) = delete;
};


struct MoveTester {
    std::string action;

    MoveTester() : action("DC") {}
    MoveTester(const MoveTester&) : action("CC") {}
    MoveTester(MoveTester&&) noexcept : action("MC") {}

    MoveTester& operator=(const MoveTester&) { action = "CA"; return *this; }
    MoveTester& operator=(MoveTester&&) noexcept { action = "MA"; return *this; }
};

struct ArgForwardTester {
    std::string actions;

    ArgForwardTester(MoveTester a, MoveTester b, MoveTester c) {
        actions = a.action + b.action + c.action;
    }
};


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
        task::list<int> list;
        list.push_back(42);
        ASSERT_TRUE(list.size() == 1)
        list.push_back(2);
        list.pop_front();
        ASSERT_TRUE(list.front() == 2)
        list.pop_back();
        ASSERT_TRUE(list.empty())
    }


    {
        task::list<std::string> list;
        list.push_front("test");
        ASSERT_TRUE(!list.empty())
        list.push_front("test2");
        ASSERT_TRUE(list.back() == "test")
        list.clear();
        list.clear();
        ASSERT_TRUE(list.size() == 0)
        list.resize(10);
        ASSERT_TRUE(list.size() == 10)
        ASSERT_TRUE(list.back() == "")
    }

    {
        const task::list<int> list(5);
        ASSERT_TRUE(list.front() == int())
        ASSERT_TRUE(list.back() == int())
    }


    {
        task::list<Immovable> list(5);
        task::list<Immovable> list2 = std::move(list);
        list = std::move(list2);
        list2 = task::list<Immovable>(10);
        list.swap(list2);
    }


    {
        task::list<MoveTester> list(5);
        list.push_back(MoveTester());
        list.push_front(MoveTester());
        list.insert(std::next(list.begin()), MoveTester());
        ASSERT_TRUE_MSG(list.back().action == "MC", "rvalue push_back")
        ASSERT_TRUE_MSG(list.back().action == "MC", "rvalue push_front")
        ASSERT_TRUE_MSG(std::next(list.cbegin())->action == "MC", "rvalue insert")

        /*task::list<ArgForwardTester> list2;
        MoveTester mt;    // reusable after move because object is left valid
        list2.emplace_back(MoveTester(), mt, std::move(mt));
        list2.emplace_front(mt, std::move(mt), MoveTester());
        list2.emplace(std::next(list2.begin()), std::move(mt), MoveTester(), mt);
        ASSERT_TRUE_MSG(list2.back().actions == "MCCCMC", "emplace_back")
        ASSERT_TRUE_MSG(list2.front().actions == "CCMCMC", "emplace_front")
        ASSERT_TRUE_MSG(std::next(list2.begin())->actions == "MCMCCC", "emplace")*/
    }


    {
        task::list<size_t> list_task(10, 30);
        std::list<size_t> list_std(10, 30);
        ASSERT_EQUAL_MSG(list_task, list_std, "Count-value constructor")

        list_task.insert(list_task.begin(), 20);
        list_std.insert(list_std.begin(), 20);

        list_task.insert(list_task.end(), 10, 20);
        list_std.insert(list_std.end(), 10, 20);

        ASSERT_EQUAL_MSG(list_task, list_std, "list::insert")

        list_task.erase(list_task.begin(), std::next(list_task.begin(), 5));
        list_std.erase(list_std.begin(), std::next(list_std.begin(), 5));

        list_task.erase(std::prev(list_task.end(), 5), list_task.end());
        list_std.erase(std::prev(list_std.end(), 5), list_std.end());

        ASSERT_EQUAL_MSG(list_task, list_std, "list::erase")
    }
/*

    {
        task::list<size_t> list;
        RandomFill(list, RandomUInt(1000, 5000));
        list.sort();
        ASSERT_TRUE(std::is_sorted(list.begin(), list.end()))

        task::list<size_t> list2 = list;
        ASSERT_EQUAL_MSG(list, list2, "Assignment operator")

        list2.resize(0);
        for (auto it = list.crbegin(); it != list.crend(); ++it) {
            list2.push_back(*it);
        }
        list.reverse();
        ASSERT_EQUAL_MSG(list, list2, "list::reverse / const reverse iterator")

        std::reverse(list.begin(), list.end());
        list2.reverse();

        ASSERT_EQUAL_MSG(list, list2, "std::reverse")
        ASSERT_TRUE(std::is_sorted(list.begin(), list.end()))

        task::list<size_t> list3(10);
        list = list2 = list3;
        ASSERT_EQUAL_MSG(list, list3, "Assignment operator")
    }


    {
        task::list<size_t> list_task;
        std::list<size_t> list_std;

        RandomFill(list_std, RandomUInt(1000, 5000), 100);

        list_task.resize(list_std.size());
        std::copy(list_std.begin(), list_std.end(), list_task.begin());

        ASSERT_EQUAL_MSG(list_task, list_std, "std::copy")

        list_task.sort();
        list_std.sort();

        ASSERT_EQUAL_MSG(list_task, list_std, "list::sort")

        list_task.unique();
        list_std.unique();

        ASSERT_EQUAL_MSG(list_task, list_std, "list::unique")

        task::list<size_t> list_task2;
        std::list<size_t> list_std2;

        RandomFill(list_std2, RandomUInt(100, 500), 100);

        list_task2.resize(list_std2.size());
        std::copy(list_std2.begin(), list_std2.end(), list_task2.begin());

        {
            auto list_task_temp = list_task2;
            auto list_std_temp = list_std2;

            auto& element_reference = list_task2.front();

            list_task.splice(++list_task.begin(), list_task2);
            list_std.splice(++list_std.begin(), list_std2);

            element_reference = 101;
            *++list_std.begin() = 101;

            ASSERT_EQUAL_MSG(list_task, list_std, "list::splice")

            list_task2 = std::move(list_task_temp);
            list_std2 = std::move(list_std_temp);

            ASSERT_EQUAL_MSG(list_task2, list_std2, "move operator=")
        }

        list_task.remove(list_task.front());
        list_std.remove(list_std.front());

        ASSERT_EQUAL_MSG(list_task, list_std, "list::remove")

        list_task.sort();
        list_std.sort();
        list_task2.sort();
        list_std2.sort();

        list_task.swap(list_task2);
        list_std.swap(list_std2);

        ASSERT_EQUAL_MSG(list_task2, list_std2, "list::swap")

        {
            auto &element_reference = list_task2.front();

            list_task.merge(list_task2);
            list_std.merge(list_std2);

            ASSERT_EQUAL_MSG(list_task, list_std, "list::merge")
            ASSERT_EQUAL_MSG(list_task2, list_std2, "list::merge")

            element_reference = 1000;

            ASSERT_TRUE_MSG(std::find(list_task.begin(), list_task.end(), 1000) != list_task.end(), "list::merge")
        }
    }

    {
        const size_t LIST_COUNT = 5;
        const size_t ITER_COUNT = 4000;
        const size_t MAX_SIZE = 1000;

        std::vector<task::list<size_t>> lists_task(LIST_COUNT);
        std::vector<std::list<size_t>> lists_std(LIST_COUNT);

        for (size_t iter = 0; iter < ITER_COUNT; ++iter) {
            for (size_t list = 0; list < LIST_COUNT; ++list) {
                if (TossCoin()) {
                    // Random Push
                    size_t count = RandomUInt(0, MAX_SIZE - lists_task[list].size());
                    for (; count; --count) {
                        auto val = RandomUInt();
                        if (TossCoin()) {
                            lists_task[list].push_back(val);
                            lists_std[list].push_back(val);
                        } else {
                            lists_task[list].push_front(val);
                            lists_std[list].push_front(val);
                        }
                    }
                } else {
                    // Random Pop
                    size_t count = RandomUInt(0, lists_task[list].size());
                    for (; count; --count) {
                        if (TossCoin()) {
                            lists_task[list].pop_back();
                            lists_std[list].pop_back();
                        } else {
                            lists_task[list].pop_front();
                            lists_std[list].pop_front();
                        }
                    }
                }

                switch (RandomUInt(4)) {
                    case 0:
                        if (!lists_std[list].empty()) {
                            lists_task[list].remove(lists_task[list].back());
                            lists_std[list].remove(lists_std[list].back());
                        }
                        break;
                    case 1:
                        lists_task[list].reverse();
                        lists_std[list].reverse();
                        break;
                    case 2:
                        lists_task[list].sort();
                        lists_std[list].sort();
                        break;
                }

                ASSERT_EQUAL_MSG(lists_task[list], lists_std[list], "Stress test")
            }
        }
    }
*/
}

#ifndef MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H
#define MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H

#include "xor_sum.h"
#include "binary.h"

template<size_t heapNumber, size_t binNumber, size_t Head, size_t... Tail>
struct FindIndex {
    static constexpr bool isGood = GetBit<binNumber, Head>::value;
    static constexpr size_t
        index = (isGood ? heapNumber : FindIndex<heapNumber + 1, binNumber, Tail...>::index);
    static constexpr size_t head = (isGood ? Head : FindIndex<heapNumber + 1, binNumber, Tail...>::head);
};

template<size_t heapNumber, size_t binNumber, size_t Head>
struct FindIndex<heapNumber, binNumber, Head> {
    static constexpr size_t index = heapNumber;
    static constexpr size_t head = Head;
};

template<size_t xorResult, size_t Heap, size_t iter>
struct CountFirst {
    static constexpr bool bitOnXor = GetBit<iter, xorResult>::value;
    static constexpr bool bitOnHeap = GetBit<iter, Heap>::value;
    static constexpr size_t count =
        (bitOnXor ? (bitOnHeap ? 0 : 1 << iter) : (bitOnHeap ? 1 << iter : 0))
            + CountFirst<xorResult, Heap, iter - 1>::count;
};

template<size_t xorResult, size_t Heap>
struct CountFirst<xorResult, Heap, 0> {
    static constexpr bool bitOnXor = GetBit<0, xorResult>::value;
    static constexpr bool bitOnHeap = GetBit<0, Heap>::value;
    static constexpr size_t count = (
        (bitOnXor ? (bitOnHeap ? 0 : 1) : (bitOnHeap ? 1 : 0))
    );
};

template<size_t Heap, size_t iterLeft, size_t iterRight>
struct CountSecond {
    static constexpr bool bitOnHeap = GetBit<iterRight, Heap>::value;
    static constexpr size_t count =
        (bitOnHeap ? 1 << iterRight : 0) + (CountSecond<Heap, iterLeft, iterRight - 1>::count);
};

template<size_t Heap, size_t iterRight>
struct CountSecond<Heap, iterRight, iterRight> {
    static constexpr size_t count = 0;
};

template<size_t xorResult, size_t Heap>
struct Count {
    static constexpr size_t count = Heap - (
        CountFirst<xorResult, Heap, Log2<xorResult>::value - 1>::count +
        CountSecond<Heap, Log2<xorResult>::value - 1, Log2<Heap>::value - 1>::count);
};

template<size_t xorResult, size_t... Heaps>
struct Win {
    static constexpr size_t index = FindIndex<0, Log2<xorResult>::value - 1, Heaps...>::index;
    static constexpr size_t count =
        Count<xorResult, FindIndex<0, Log2<xorResult>::value - 1, Heaps...>::head>::count;
};

template<size_t... Heaps>
struct Game {
    static constexpr size_t xorResult = XorSum<Heaps...>::value;
    static constexpr bool first_player_wins = xorResult != 0;
    static constexpr ssize_t first_move_heap_index =
        first_player_wins ? Win<xorResult, Heaps...>::index : -1;
    static constexpr ssize_t first_move_rocks_count =
        first_player_wins ? Win<xorResult, Heaps...>::count : -1;
};

#endif /// MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H.

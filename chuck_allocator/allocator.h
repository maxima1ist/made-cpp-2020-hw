#pragma once
#include <iostream>

template<class T>
class Allocator {
private:

    struct Chunk {
        Chunk* next;
        char* memory;
        char* currentAddress;
        std::size_t allocatorsCount;

        explicit Chunk(const std::size_t& allocatorsCount)
            : memory(new char[CHUNK_SIZE]), currentAddress(memory), next(nullptr), allocatorsCount(allocatorsCount) {}

        char* getAllocatedBlock(const std::size_t& size) {
            if (memory + CHUNK_SIZE - currentAddress >= size) {
                char* buffer = currentAddress;
                currentAddress += size;
                return buffer;
            } else {
                return nullptr;
            }
        }

        void deallocateBlock(char* p, const std::size_t& size) {}

        bool hasPointer(const char* p, const std::size_t& size) const {
            return (memory <= p) && (p <= memory + CHUNK_SIZE) && (memory + CHUNK_SIZE - p >= size);
        }

        ~Chunk() {
            delete[] memory;
        }
    };

    Chunk* currentChunk;

    void destroyAllocator() {
        while (currentChunk != nullptr) {
            Chunk* buffer = currentChunk->next;
            if (currentChunk->allocatorsCount == 1) {
                delete currentChunk;
            } else {
                --(currentChunk->allocatorsCount);
            }
            currentChunk = buffer;
        }
        currentChunk = nullptr;
    }

    void copyAllocator(const Allocator& copy) {
        currentChunk = copy.currentChunk;
        Chunk* bufferCopy = copy.currentChunk;
        while (bufferCopy != nullptr) {
            ++(bufferCopy->allocatorsCount);
            bufferCopy = bufferCopy->next;
        }
    }

public:

    typedef T value_type;
    typedef T* pointer;
    typedef const pointer const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    static const size_type CHUNK_SIZE = 1 << 9;

    template<class U>
    struct rebind {
        typedef Allocator<U> other;
    };

    Allocator() : currentChunk(nullptr) {}

    Allocator(const Allocator& copy) : currentChunk(nullptr) {
        copyAllocator(copy);
    }

    ~Allocator() {
        destroyAllocator();
    }

    Allocator& operator=(const Allocator& copy) {
        if (this == &copy) {
            return *this;
        }

        destroyAllocator();
        copyAllocator(copy);

        return *this;
    }

    pointer allocate(const size_type& size) {
        if (size > CHUNK_SIZE) {
            throw "Incorrect size!";
        }

        if (currentChunk != nullptr) {
            size_type allocatorsCount = currentChunk->allocatorsCount;

            Chunk* buffer = currentChunk;
            char* allocatedBlock;
            while (buffer->next != nullptr) {
                allocatedBlock = buffer->getAllocatedBlock(size * sizeof(T));
                if (allocatedBlock != nullptr) {
                    return (pointer) allocatedBlock;
                }
                buffer = buffer->next;
            }

            allocatedBlock = buffer->getAllocatedBlock(size * sizeof(T));
            if (allocatedBlock != nullptr) {
                return (pointer) allocatedBlock;
            } else {
                Chunk* newChunk = new Chunk(allocatorsCount);
                allocatedBlock = newChunk->getAllocatedBlock(size * sizeof(T));
                buffer->next = newChunk;
                return (pointer) allocatedBlock;
            }
        } else {
            currentChunk = new Chunk(1);
            char* allocatedBlock = currentChunk->getAllocatedBlock(size * sizeof(T));
            return (pointer) allocatedBlock;
        }
    }

    void deallocate(pointer p, const size_type& size) {}

    template<typename U, typename... Args>
    void construct(U* ptr, Args&& ... args) {
        new((U*) ptr) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

};
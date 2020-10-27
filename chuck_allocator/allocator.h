#pragma once
#include <iostream>

template<class T>
class Allocator {
private:
    static const std::size_t CHUNK_SIZE = 1 << 9;

    struct Chunk {
        Chunk* prev;
        char* memory;
        char* currentAddress;
        std::size_t allocatorsCount;

        explicit Chunk(const size_t& allocatorsCount)
            : memory(new char[CHUNK_SIZE]), currentAddress(memory), prev(nullptr), allocatorsCount(allocatorsCount) {}

        char* getAllocatedBlock(const size_t& size) {
            if (memory + CHUNK_SIZE - currentAddress >= size) {
                char* buffer = currentAddress;
                currentAddress += size;
                return buffer;
            } else {
                return nullptr;
            }
        }

        void deallocateBlock(char* p, const size_t& size) {
            //сложна
        }

        bool hasPointer(const char* p, const size_t& size) const {
            return (memory <= p) && (p <= memory + CHUNK_SIZE) && (memory + CHUNK_SIZE - p >= size);
        }

        ~Chunk() {
            delete[] memory;
        }
    };

    Chunk* currentChunk;

    void destroyAllocator() {
        while (currentChunk != nullptr) {
            Chunk* buffer = currentChunk->prev;
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
            bufferCopy = bufferCopy->prev;
        }
    }

public:

    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

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

    T* allocate(const std::size_t& size) {
        if (size > CHUNK_SIZE) {
            throw "Incorrect size!";
        }

        if (currentChunk != nullptr) {
            size_t allocatorsCount = currentChunk->allocatorsCount;

            Chunk* buffer = currentChunk;
            char* allocatedBlock;
            while (buffer->prev != nullptr) {
                allocatedBlock = buffer->getAllocatedBlock(size * sizeof(T));
                if (allocatedBlock != nullptr) {
                    return (T*) allocatedBlock;
                }
                buffer = buffer->prev;
            }

            allocatedBlock = buffer->getAllocatedBlock(size * sizeof(T));
            if (allocatedBlock != nullptr) {
                return (T*) allocatedBlock;
            } else {
                Chunk* newChunk = new Chunk(allocatorsCount);
                allocatedBlock = newChunk->getAllocatedBlock(size * sizeof(T));
                newChunk->prev = currentChunk;
                currentChunk = newChunk;
                return (T*) allocatedBlock;
            }
        } else {
            currentChunk = new Chunk(1);
            char* allocatedBlock = currentChunk->getAllocatedBlock(size * sizeof(T));
            return (T*) allocatedBlock;
        }
    }

    void deallocate(T* p, const std::size_t& size) {
        /*
        if (p == nullptr || size > CHUNK_SIZE) {
            throw "Incorrect pointer or size!";
        }

        char* deallocationPointer = (char*) p;
        Chunk* buffer = currentChunk;
        while (buffer != nullptr) {
            if (buffer->hasPointer(deallocationPointer, size)) {
                buffer->deallocateBlock(deallocationPointer, size);
                return;
            }
            buffer = buffer->prev;
        }

        throw "Can not to deallocate size of memory in such pointer!";
         */
    }

    template<typename U, typename... Args>
    void construct(U* ptr, Args&& ... args) {
        new((U*) ptr) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

};
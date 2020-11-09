#pragma once

namespace task {

    template<class T>
    class UniquePtr;

    template<class T>
    struct PointerController;

    template<class T>
    class SharedPtr;

    template<class T>
    class WeakPtr;

    template<class T>
    class UniquePtr {
    private:

        T* ptr;

    public:

        UniquePtr() noexcept;

        explicit UniquePtr(T*) noexcept;

        UniquePtr(UniquePtr<T>&) noexcept = delete;
        UniquePtr(UniquePtr<T>&&) noexcept;

        UniquePtr& operator=(UniquePtr<T>&) noexcept = delete;
        UniquePtr& operator=(UniquePtr<T>&&) noexcept;

        ~UniquePtr();

        T* get() const noexcept;

        T& operator*() const;
        T* operator->() const noexcept;

        T* release() noexcept;
        void reset(T*) noexcept;

        void swap(UniquePtr<T>&) noexcept;

    };

    template<class T>
    struct PointerController {

        T* ptr;
        std::size_t pointerCount;

        explicit PointerController(T*) noexcept;

        void removeController() noexcept;

        ~PointerController() = default;

    };

    template<class T>
    class SharedPtr {
        friend class WeakPtr<T>;

    private:

        PointerController<T>* ptrController;

    public:

        SharedPtr() noexcept;

        explicit SharedPtr(T*) noexcept;

        SharedPtr(const SharedPtr<T>&) noexcept;

        SharedPtr(const WeakPtr<T>&) noexcept;

        SharedPtr(SharedPtr<T>&&) noexcept;

        SharedPtr& operator=(const SharedPtr<T>&) noexcept;

        SharedPtr& operator=(SharedPtr<T>&&) noexcept;

        ~SharedPtr();

        T* get() const noexcept;

        T& operator*() const noexcept;

        T* operator->() const noexcept;

        std::size_t use_count() const;

        void reset() noexcept;

        void reset(T*) noexcept;

        void swap(SharedPtr<T>&) noexcept;

    };

    template<class T>
    class WeakPtr {
    private:

        PointerController<T>* ptrController;

    public:

        WeakPtr() noexcept;

        WeakPtr(SharedPtr<T>&) noexcept;

        WeakPtr(const WeakPtr<T>&) noexcept;

        WeakPtr(WeakPtr<T>&&) noexcept;

        WeakPtr& operator=(const WeakPtr<T>&) noexcept;

        WeakPtr& operator=(WeakPtr<T>&&) noexcept;

        WeakPtr& operator=(SharedPtr<T>&) noexcept;

        ~WeakPtr() = default;

        std::size_t use_count() const noexcept;

        bool expired() const noexcept;

        SharedPtr<T> lock() const noexcept;

        void reset() noexcept;

    };

    template<class T>
    void swap(WeakPtr<T>&, WeakPtr<T>&) noexcept;

}  // namespace task


#include "smart_pointers.tpp"

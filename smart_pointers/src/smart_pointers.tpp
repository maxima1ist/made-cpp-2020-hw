#include "smart_pointers.h"

namespace task {

    template<class T>
    UniquePtr<T>::UniquePtr() noexcept: ptr(nullptr) {}

    template<class T>
    UniquePtr<T>::UniquePtr(T* ptr) noexcept: ptr(ptr) {}

    template<class T>
    UniquePtr<T>::UniquePtr(UniquePtr<T>&& other) noexcept: ptr(std::move(other.ptr)) {
        other.ptr = nullptr;
    }

    template<class T>
    UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<T>&& other) noexcept {
        ptr = std::move(other.ptr);
        other.ptr = nullptr;
        return *this;
    }

    template<class T>
    UniquePtr<T>::~UniquePtr<T>() {
        delete ptr;
    }

    template<class T>
    T* UniquePtr<T>::get() const noexcept {
        return ptr;
    }

    template<class T>
    T& UniquePtr<T>::operator*() const {
        return *get();
    }

    template<class T>
    T* UniquePtr<T>::operator->() const noexcept {
        return get();
    }

    template<class T>
    T* UniquePtr<T>::release() noexcept {
        T* buffer = ptr;
        ptr = nullptr;
        return buffer;
    }

    template<class T>
    void UniquePtr<T>::reset(T* ptr) noexcept {
        delete this->ptr;
        this->ptr = ptr;
    }

    template<class T>
    void UniquePtr<T>::swap(UniquePtr<T>& other) noexcept {
        std::swap(ptr, other.ptr);
    }

    template<class T>
    PointerController<T>::PointerController(T* ptr) noexcept : ptr(ptr), pointerCount(1) {}

    template<class T>
    void PointerController<T>::removeController() noexcept {
        if (pointerCount == 1) {
            delete ptr;
        }
        --pointerCount;
    }

    template<class T>
    SharedPtr<T>::SharedPtr() noexcept : ptrController(nullptr) {}

    template<class T>
    SharedPtr<T>::SharedPtr(T* ptr) noexcept: ptrController(new PointerController(ptr)) {}

    template<class T>
    SharedPtr<T>::SharedPtr(const SharedPtr<T>& sharedPtr) noexcept: ptrController(sharedPtr.ptrController) {
        if (ptrController) {
            ++(ptrController->pointerCount);
        }
    }
    template<class T>
    SharedPtr<T>::SharedPtr(const WeakPtr<T>& weakPtr) noexcept: ptrController(weakPtr.lock()) {
        if (ptrController) {
            ++(ptrController->pointerCount);
        }
    }
    template<class T>
    SharedPtr<T>::SharedPtr(SharedPtr<T>&& other) noexcept: ptrController(std::move(other.ptrController)) {
        other.ptrController = nullptr;
    }

    template<class T>
    SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& other) noexcept {
        if (&other == this) {
            return *this;
        }

        if (ptrController) {
            ptrController->removeController();
        }
        ptrController = other.ptrController;
        if (ptrController) {
            ++(ptrController->pointerCount);
        }

        return *this;
    }

    template<class T>
    SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& other) noexcept {
        if (ptrController) {
            ptrController->removeController();
        }
        ptrController = std::move(other.ptrController);
        if (ptrController) {
            other.ptrController = nullptr;
        }

        return *this;
    }

    template<class T>
    SharedPtr<T>::~SharedPtr() {
        if (ptrController) {
            ptrController->removeController();
        }
    }

    template<class T>
    T* SharedPtr<T>::get() const noexcept {
        return (ptrController ? ptrController->ptr : nullptr);
    }

    template<class T>
    T& SharedPtr<T>::operator*() const noexcept {
        return *get();
    }

    template<class T>
    T* SharedPtr<T>::operator->() const noexcept {
        return get();
    }

    template<class T>
    std::size_t SharedPtr<T>::use_count() const {
        return (ptrController ? ptrController->pointerCount : 0);
    }

    template<class T>
    void SharedPtr<T>::reset() noexcept {
        if (ptrController) {
            ptrController->removeController();
        }
        ptrController = nullptr;
    }

    template<class T>
    void SharedPtr<T>::reset(T* ptr) noexcept {
        if (ptrController) {
            ptrController->removeController();
        }
        ptrController = new PointerController(ptr);
    }

    template<class T>
    void SharedPtr<T>::swap(SharedPtr<T>& other) noexcept {
        std::swap(ptrController, other.ptrController);
    }

    template<class T>
    WeakPtr<T>::WeakPtr() noexcept: ptrController(nullptr) {}

    template<class T>
    WeakPtr<T>::WeakPtr(SharedPtr<T>& sharedPtr) noexcept: ptrController(sharedPtr.ptrController) {}

    template<class T>
    WeakPtr<T>::WeakPtr(const WeakPtr<T>& other) noexcept: ptrController(other.ptrController) {}

    template<class T>
    WeakPtr<T>::WeakPtr(WeakPtr<T>&& other) noexcept: ptrController(std::move(other.ptrController)) {
        other.ptrController = nullptr;
    }

    template<class T>
    WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& other) noexcept {
        if (&other == this) {
            return *this;
        }

        ptrController = other.ptrController;
        return *this;
    }

    template<class T>
    WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<T>&& other) noexcept {
        ptrController = std::move(other.ptrController);
        other.ptr = nullptr;
        return *this;
    }

    template<class T>
    WeakPtr<T>& WeakPtr<T>::operator=(SharedPtr<T>& other) noexcept {
        ptrController = other.ptrController;
        return *this;
    }

    template<class T>
    std::size_t WeakPtr<T>::use_count() const noexcept {
        return (ptrController ? ptrController->pointerCount : 0);
    }

    template<class T>
    bool WeakPtr<T>::expired() const noexcept {
        return use_count() == 0;
    }

    template<class T>
    SharedPtr<T> WeakPtr<T>::lock() const noexcept {
        if (expired()) {
            return SharedPtr<T>();
        } else {
            SharedPtr<T> buffer;
            buffer.ptrController = ptrController;
            ++(ptrController->pointerCount);
            return buffer;
        }
    }

    template<class T>
    void WeakPtr<T>::reset() noexcept {
        ptrController = nullptr;
    }

    template<class T>
    void swap(WeakPtr<T>& lhs, WeakPtr<T>& rhs) noexcept {
        lhs.swap(rhs);
    }

}

#pragma once

#include <utility>
#include <memory>

namespace task {
    template<typename Signature>
    class function;

    template<typename ReturnType, typename ...Args>
    class function<ReturnType(Args...)> {
    public:

        using result_type = ReturnType;

    private:

        struct callable_base {
        public:
            callable_base() noexcept = default;
            virtual ~callable_base() = default;
            virtual result_type invoke(Args ... args) const = 0;
            virtual const std::type_info& get_target_type() const noexcept = 0;
        };

        template<typename Functor>
        struct callable : public callable_base {
            std::unique_ptr<Functor> funcPtr;
            explicit callable(const Functor func) : callable_base(), funcPtr(new Functor(func)) {}

            result_type invoke(Args ...args) const override {
                return (*funcPtr)(args...);
            }

            const std::type_info& get_target_type() const noexcept override {
                return typeid(Functor);
            }

            Functor* get_func() const noexcept {
                return funcPtr.get();
            }
        };

        using callablePointerType = std::shared_ptr<callable_base>;

        callablePointerType funcP;

    public:

        function() noexcept: funcP(nullptr) {}
        explicit function(nullptr_t) noexcept: funcP(nullptr) {}
        function(const function& other) : funcP(other.funcP) {}
        function(function&& other) noexcept: funcP(std::forward<callablePointerType>(other.funcP)) {}
        ~function() = default;

        template<typename Functor>
        function(const Functor func) : funcP(new callable<Functor>(func)) {}

        function& operator=(const function& other) {
            if (this == &other) {
                return *this;
            }

            function(other).swap(*this);
            return *this;
        }

        function& operator=(function&& other) noexcept {
            funcP = std::forward<callablePointerType>(other.funcP);
            return *this;
        }

        function& operator=(nullptr_t) {
            funcP = nullptr;
            return *this;
        }

        template<class Functor>
        function& operator=(Functor&& func) {
            function(std::forward<Functor>(func)).swap(*this);
            return *this;
        }

        template<class Functor>
        function& operator=(std::reference_wrapper<Functor> func) noexcept {
            function(func).swap(*this);
        }

        void swap(function& other) noexcept {
            std::swap(*this, other);
        }

        explicit operator bool() const noexcept {
            return funcP != nullptr;
        }

        result_type operator()(Args ...args) const {
            if (!funcP) {
                throw "bad function call";
            }
            return funcP->invoke(args...);
        }

        const std::type_info& target_type() const noexcept {
            if (funcP) {
                return funcP.get()->get_target_type();
            } else {
                return typeid(void);
            }
        }

        template<class T>
        T* target() noexcept {
            if (funcP) {
                return dynamic_cast<callable<T>*>(funcP.get())->get_func();
            } else {
                return nullptr;
            }
        }

        template<class T>
        const T* target() const noexcept {
            return target<T>();
        }

    };
}
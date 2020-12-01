#pragma once

template<class Func, typename ...Other>
struct Composer {
public:
    typedef Func returnFunc;

private:
    Func func;
    Composer<Other...> other;

public:
    explicit Composer(Func func, Other ...other) : func(func), other(other...) {
        static_assert(std::is_same<
            typename Composer<Other...>::returnFunc::result_type,
            typename Func::argument_type
        >::value, "types error!");
    }

    template<class Arg>
    auto operator()(const Arg& arg) {
        return func(other(arg));
    }
};

template<class Func1, class Func2>
struct Composer<Func1, Func2> {
public:
    typedef Func1 returnFunc;

private:
    Func1 func1;
    Func2 func2;

public:
    explicit Composer(Func1 func1, Func2 func2) : func1(func1), func2(func2) {
        static_assert(std::is_same<
            typename Func2::result_type,
            typename Func1::argument_type
        >::value, "types error!");
    }

    template<class Arg>
    auto operator()(const Arg& arg) {
        return func1(func2(arg));
    }
};

template<class Func>
struct Composer<Func> {
private:
    Func func;

public:
    explicit Composer(Func func) : func(func) {}

    //about ...
    //may be excessively, but it's really logical, that the only one can be with a variable number of arguments
    //cause it's just one function, not a compose
    //but if it was not true logic it would be really easy to fix just erasing "..." everywhere
    template<class ...Arg>
    auto operator()(const Arg& ...arg) {
        return func(arg...);
    }
};

template<class ...Funcs>
auto compose(Funcs ...funcs) {
    return Composer<Funcs...>(funcs...);
}

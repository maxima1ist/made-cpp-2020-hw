#pragma once
#include <functional>

template<typename ReturnType, typename ...Argument>
std::function<ReturnType(Argument...)> compose(const std::function<ReturnType(Argument...)>& func) {
    return func;
}

template<typename ReturnType1, typename TheSameType, typename ...Argument2>
std::function<ReturnType1(Argument2...)> compose(
    const std::function<ReturnType1(TheSameType)>& func1,
    const std::function<TheSameType(Argument2...)>& func2
) {
    std::function<ReturnType1(Argument2...)> result = [func1, func2](auto ...arg) { return func1(func2(arg...)); };
    return result;
}

template<typename ReturnType1, typename TheSameType, typename Argument2, typename ...Other>
std::function<ReturnType1(Argument2)> compose(
    const std::function<ReturnType1(TheSameType)>& func1,
    const std::function<TheSameType(Argument2)>& func2,
    Other ...other
) {
    std::function<ReturnType1(Argument2)> result = [func1, func2](auto arg) { return func1(func2(arg)); };
    return compose(result, other...);
}

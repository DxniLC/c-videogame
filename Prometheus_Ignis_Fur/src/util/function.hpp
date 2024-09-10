#pragma once

#include <functional>

struct FunctionBase
{
    virtual ~FunctionBase() = default;
};

template <typename... Args>
struct Function : FunctionBase
{
    explicit Function(std::function<bool(Args...)> func) : func(func) {}
    
    std::function<bool(Args...)> func;
};
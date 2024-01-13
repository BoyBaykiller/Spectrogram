#pragma once
#include <iostream>
#include <format>

namespace Logger
{
    template <typename... TTypes>
    inline void Log(std::format_string<TTypes...> format, TTypes&&... args)
    {
        auto formatted = std::format(format, std::forward<TTypes>(args)...);
        std::cout << formatted << '\n';
    };

    template <typename T>
    inline void Log(T arg)
    {
        std::cout << arg << '\n';
    };


    inline void Log(std::string_view message)
    {
        std::cout << message << '\n';
    };
}
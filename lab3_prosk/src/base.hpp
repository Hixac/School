#pragma once

#include <array>

template<typename T, unsigned long long __size_extern, unsigned long long __size_inner>
using ArrTwo = std::array<std::array<T, __size_inner>, __size_extern>;

using size_t = std::size_t;

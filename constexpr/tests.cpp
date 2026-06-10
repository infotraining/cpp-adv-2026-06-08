#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <array>

namespace Cpp11
{
    constexpr int factorial(int n)
    {
        return (n == 0) ? 1 : n * factorial(n - 1);
    }
} // namespace Cpp11

constexpr int fibonacci(int n)
{
    if (n == 0 || n == 1)
    {
        return n;
    }

    return fibonacci(n-1) + fibonacci(n-2); 
}

TEST_CASE("constexpr")
{
    constexpr int f_5 = Cpp11::factorial(5);

    int tab1[Cpp11::factorial(6)] = {};

    SECTION("runtime")
    {
        int x = 10;
        REQUIRE(fibonacci(10) == 55);
    }

    SECTION("compile-time")
    {
        constexpr auto fib_10 = fibonacci(10);

        int tab2[fibonacci(12)];
    }
}

template <size_t N, typename F>
constexpr auto create_lookup_table(F f)
{
    std::array<int, N> lookup_table = {};

    for(int i = 0; i < N; ++i)
        lookup_table[i] = f(i);
    
    return lookup_table;
}

int optimized_fibonacci(int n)
{
    constexpr size_t N = 20;
    static constexpr auto fibonacci_lookup_table = create_lookup_table<N>(fibonacci);

    if (n < N)
    {
        return fibonacci_lookup_table[n];
    }

    return fibonacci(n);
}

TEST_CASE("lookup tables at compile time")
{
    constexpr auto fibonacci_lookup_table = create_lookup_table<20>(fibonacci);
}
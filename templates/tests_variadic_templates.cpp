#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <tuple>

template <typename... Ts>
struct Row
{
    std::tuple<Ts...> data;

    size_t columns_size()
    {
        return sizeof...(Ts);
    }
};

template <typename T, typename... TArgs>
std::unique_ptr<T> my_make_unique(TArgs&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
}

// Head-Tail

void print()
{
    std::cout << "\n";
}

template <typename Head, typename... Tail>
void print(const Head& head, const Tail&... tail)
{
    std::cout << head << " ";
    print(tail...);
}

namespace Cpp17
{
    template <typename Head, typename... Tail>
    void print(const Head& head, const Tail&... tail)
    {
        if constexpr(sizeof...(tail) > 0)
        {
            std::cout << head << " ";
            print(tail...);
        }
        else
        {
            std::cout << "\n";
        }
    }
} // namespace Cpp17

TEST_CASE("variadic templates")
{
    Row<> r0;
    Row<int, double, std::string> r1;
    REQUIRE(r1.columns_size() == 3);
    Row<int, double, std::string, int, double, std::string, int, double, std::string> r2;
    REQUIRE(r2.columns_size() == 9);

    print(1, 3.14, "text");
    print(1, 3.14, "text", std::string("string"));
}
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <string>
#include <vector>


namespace Exercise
{
    // TODO: Implement a function that sets all elements of a container to zero (default value).

    template <typename Container>
    void zero(Container& container)
    {
        using ValueType = typename Container::value_type;

        for (auto& item : container)
        {
            item = ValueType{};
        }
    }
} // namespace Exercise

TEST_CASE("zero")
{
    using namespace Exercise;

    SECTION("vector<int>")
    {
        std::vector<int> vec = {1, 2, 3, 4};

        zero(vec);

        REQUIRE(vec == std::vector{0, 0, 0, 0});
    }

    SECTION("list<std::string>")
    {
        std::list<std::string> lst = {"one", "two", "three"};

        zero(lst);

        REQUIRE(lst == std::list<std::string>{"", "", ""});
    }

    // SECTION("native array")
    // {
    //     int native_array[10];
    //     zero(native_array);

    //     REQUIRE(std::all_of(std::begin(native_array), std::end(native_array), [](int item) { return item == 0; }));
    // }
}

TEST_CASE("auto declarations")
{
    auto av1 = 42; // int

    const int x = 665;
    auto av2 = x;  // int
}

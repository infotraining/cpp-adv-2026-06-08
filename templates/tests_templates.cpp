#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

using Helpers::Gadget;

template <typename T>
T maximum(T a, T b)
{
    return a < b ? b : a;
}

// Explicit function template specialization
// template <>
// const char* maximum<const char*>(const char* a, const char* b)
// {
//     return std::strcmp(a, b) < 0 ? b : a;
// }

const char* maximum(const char* a, const char* b)
{
    return std::strcmp(a, b) < 0 ? b : a;
}

template <typename T>
void foo(T arg)
{
    // undeclared(); // 1-st phase
    undeclared(arg); // 2-nd phase

    static_assert(sizeof(int) == 32); // 1-st phase
    static_assert(sizeof(T) == 32);   // 2-nd phase
}

TEST_CASE("function templates")
{
    SECTION("basic template functions")
    {
        int result_1 = maximum(42, 665);
        REQUIRE(result_1 == 665);

        char c = 'A';
        REQUIRE(maximum(c, 'Z') == 'Z');

        REQUIRE(maximum(3.14, 665.1) == 665.1);

        std::string str1 = "kawa";
        std::string str2 = "herbata";
        REQUIRE(maximum(str1, str2) == "kawa");
    }

    SECTION("resolving deduction conflict")
    {
        size_t size = 1024;
        REQUIRE(maximum(static_cast<int>(size), 333) == 1024);

        REQUIRE(maximum<int>(size, 333) == 1024); // deduction turned off
    }

    SECTION("function template specialization")
    {
        const char* txt1 = "Ola";
        const char* txt2 = "Ala";

        REQUIRE(maximum(txt1, txt2) == std::string("Ola"));
    }

    SECTION("address of function")
    {
        int (*ptr_fun)(int, int) = &maximum; // &maximum<int> - deduced from type of pointer

        auto other_ptr_fun = &maximum<double>; // explicit intantiation
    }
}

template <typename Iterator>
Iterator min_value(Iterator begin, Iterator end)
{
    Iterator min_value_it = begin;
    for (auto it = begin; it != end; it++)
    {
        if (*it < *min_value_it)
            min_value_it = it;
    }

    return min_value_it;
}

template <typename Container>
bool cmp_by_size(const Container& a, const Container& b)
{
    return a.size() < b.size();
}

template <typename Iterator, typename Comparator>
Iterator min_value(Iterator begin, Iterator end, Comparator isSmaller)
{
    Iterator min_value_it = begin;
    for (auto it = begin; it != end; it++)
    {
        if (isSmaller(*it, *min_value_it))
            min_value_it = it;
    }

    return min_value_it;
}

TEST_CASE("small exercise")
{
    SECTION("native arrays")
    {
        int numbers_1[10] = {4, 645, 235, 645, 756, 32, 645};

        int* start = numbers_1;
        int* end = numbers_1 + 10;

        int* pos_min = min_value(start, end);
        REQUIRE(*pos_min == 0);
    }

    SECTION("vector")
    {
        std::vector<int> numbers = {4, 645, 235, 1, 756, 32, 645};

        auto pos_min = min_value(numbers.begin(), numbers.end());
        REQUIRE(*pos_min == 1);
    }

    SECTION("list")
    {
        std::list<int> numbers = {4, 645, 235, 1, 756, 32, 645};

        auto pos_min = min_value(numbers.begin(), numbers.end());
        REQUIRE(*pos_min == 1);
    }

    SECTION("empty container")
    {
        std::vector<int> vec_empty;
        auto pos_min = std::min_element(vec_empty.begin(), vec_empty.end());
        REQUIRE(pos_min == vec_empty.end());

    }

    SECTION("min_value with comparer")
    {
        std::vector<std::string> words = {"aaa", "aa", "a", "abbb", "aaaaf"};
        auto pos_min = min_value(words.begin(), words.end(), &cmp_by_size<std::string>);
        REQUIRE(*pos_min == "a");

        std::vector<std::vector<int>> data = {{1, 2, 3}, {2, 3}, {42}, {1, 2, 3, 5, 6}};
        auto pos_smallest_vec = min_value(data.begin(), data.end(), &cmp_by_size<std::vector<int>>);
        REQUIRE(*pos_smallest_vec == std::vector<int>{42});

        pos_smallest_vec = min_value(data.begin(), data.end(), [](const std::vector<int>& a, const std::vector<int>& b) { return a.size() < b.size(); });
        REQUIRE(*pos_smallest_vec == std::vector<int>{42});
    }
}

TEST_CASE("class templates")
{
    // TODO
}

TEST_CASE("template aliases")
{
    // TODO
}

TEST_CASE("template variables")
{
    // TODO
}
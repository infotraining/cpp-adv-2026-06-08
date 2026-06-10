#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

namespace Cpp20
{
    void deduce1(auto arg)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    }
}

template <typename T>
void deduce2(T& arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void deduce3(T&& arg) // T&& - universal reference - accepts lvalue -> T& and accepts rvalue -> T&&
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

TEST_CASE("type deduction rules")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = x;
    int tab[10];

    SECTION("Case 1")
    {
        deduce1(x);         // deduce1<int>(int arg)
        deduce1(cx);        // deduce1<int>(int arg) - const is removed
        deduce1(ref_x);     // deduce1<int>(int arg) - ref is removed
        deduce1(cref_x);    // deduce1<int>(int arg) - const ref is removed
        deduce1(tab);       // deduce1<int*>(int* arg) - array decays to pointer
        deduce1(foo);       // deduce1<void(*)(int)>(void(*arg)(int)) - function decays to pointer

        auto a1 = x;        // int a1 = x;
        auto a2 = cx;       // int a2 = cx; - const is removed
        auto a3 = ref_x;    // int a3 = ref_x; - ref is removed
        auto a4 = cref_x;   // int a4 = cref_x; - const ref is removed
        auto a5 = tab;      // int* a5 = tab; - array decays to pointer
        auto a6 = foo;      // void(*a6)(int) - function decays to pointer
    }

    SECTION("Case 2")
    {
        
        deduce2(x);         // deduce2<int>(int& arg)
        deduce2(cx);        // deduce2<const int>(const int& arg)
        deduce2(ref_x);     // deduce2<int>(int& arg)
        deduce2(cref_x);    // deduce2<const int>(const int& arg)
        deduce2(tab);       // deduce2<int[10]>(int (&arg)[10])
        deduce2(foo);       // deduce2<void(int)>(void(&arg)(int))

        auto& a1 = x;       // int& a1
        auto& a2 = cx;      // const int& a2
        auto& a3 = ref_x;   // int& a3
        auto& a4 = cref_x;  // const int& a4
        auto& a5 = tab;     // int(&a5)[10]
        auto& a6 = foo;     // void(&a6)(int)
    }

    SECTION("Case 3")
    {
        // if lvalue then deduce lvalue-ref as template arg
        // if rvalue then deduce no-ref type

        deduce3(x);           // deduce3<int&>(int& && -> int&)                   
        deduce3(cx);          // deduce3<const int&>(const int& && -> const int&)                        
        deduce3(ref_x);       // deduce3<int&>(int& && -> int&)                                           
        deduce3(cref_x);      // deduce3<const int&>(const int& && -> const int&)                         
        deduce3(tab);         // deduce3<int(&)[10]>(int(&)[10] && -> int(&)[10])                        
        deduce3(foo);         // deduce3<void(&)(int)>(void(&)(int) && -> void(&)(int))                                                
        deduce3(string("text")); // deduce3<std::string>(std::string&&)                

        auto&& a1 = x;           // int& a1 = x;
        auto&& a2 = cx;          // const int& a2 = cx;
        auto&& a3 = ref_x;       // int& a3 = ref_x;
        auto&& a4 = cref_x;      // const int& a4 = cref_x;
        auto&& a5 = tab;         // int (&a5)[10] = tab;
        auto&& a6 = foo;         // void (&a6)(int) = foo;
        auto&& a7 = string("text"); // int&& a7 = string("text");
    }
}

namespace Exercise
{
    template <typename T>
    auto begin(const T& container)
    {
        return container.begin();
    }

    template <typename T, size_t N>
    auto begin(T (&tab)[N])
    {
        return tab;
    }

    template <typename T>
    auto end(const T& container)
    {
        return container.end();
    }

    template <typename T, size_t N>
    auto end(T (&tab)[N])
    {
        return tab + N;
    }

    template <typename T>
    size_t size(const T& container)
    {
        return container.size();
    }

    template <typename T, size_t N>
    size_t size(T (&tab)[N])
    {
        return N;
    }
}

TEST_CASE("how to deduce size of static array")
{
    int tab1[1024];
    const char tab2[] = {'a', 'b', 'c', '\0'};
    std::vector<int> vec = {1, 2, 3};

    REQUIRE(Exercise::size(tab1) == 1024);
    REQUIRE(Exercise::size(tab2) == 4);
    REQUIRE(Exercise::size("text") == 5);
    REQUIRE(Exercise::size(vec) == 3);

    auto pos_b = std::find(std::begin(tab2), std::end(tab2), 'b');
}

namespace ReferenceCollapsing
{
    template <typename T>
    void collapse(T& arg)
    {}
}

TEST_CASE("Reference collapsing")
{
    using ReferenceCollapsing::collapse;

    int x = 42;
    collapse<int&>(x);  // collapse: int& & -> int&

    collapse<int&&>(x); // collapse: int&& & -> int&
}

TEST_CASE("decltype(auto)")
{ 
}
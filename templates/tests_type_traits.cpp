#include <catch2/catch_test_macros.hpp>

int identity(int n)
{
    return n;
}

TEST_CASE("classic function")
{
    REQUIRE(identity(42) == 42);
}

//////////////////////////////////////
// IdentityValue

template <int N>
struct IdentityValue
{
    static constexpr int value = N;
};

template <int N>
constexpr int IdentityValue_v = IdentityValue<N>::value;

//////////////////////////////////////
// IdentityType

template <typename T>
struct IdentityType
{
    using type = T;
};

template <typename T>
using IdentityType_t = typename IdentityType<T>::type;

//////////////////////////////////////
// IsSame

template <typename T1, typename T2>
struct IsSame
{
    static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T>
{
    static constexpr bool value = true;
};

template <typename T1, typename T2>
constexpr bool IsSame_v = IsSame<T1, T2>::value;

//////////////////////////////////////////
// RemoveReference

template <typename T>
struct RemoveReference
{
    using type = T;
};

template <typename T>
struct RemoveReference<T&>
{
    using type = T;
};

template <typename T>
struct RemoveReference<T&&>
{
    using type = T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::type;

//////////////////////////////////////
// IsPointer

template <typename T>
struct IsPointer : std::false_type
{};


template <typename T>
struct IsPointer<T*> : std::true_type
{};

template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

TEST_CASE("type traits")
{   
    static_assert(IdentityValue<42>::value == 42);
    static_assert(IdentityValue_v<42> == 42);

    IdentityType<int>::type x = 24; // int x = 24; 
    IdentityType_t<double> dx = 0.01;

    using T1 = int;
    using T2 = int;
    using T3 = double;

    static_assert(IsSame<T1, T2>::value);
    static_assert(IsSame_v<T1, T2>);
    static_assert(not IsSame<T1, T3>::value);
    static_assert(not IsSame_v<T1, T3>);

    static_assert(IsSame_v<RemoveReference_t<int&>, int>);

    using T4 = int*;

    static_assert(IsPointer_v<T4>);
    static_assert(not IsPointer_v<T1>);

    static_assert(std::true_type::value);
    static_assert(not std::false_type::value);
}
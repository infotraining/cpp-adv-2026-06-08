#include <catch2/catch_test_macros.hpp>

namespace Explain
{
    template <bool Condition, typename T = void>
    struct EnableIf
    {
        using type = T;
    };

    template <typename T>
    struct EnableIf<false, T>
    { };

    template <bool Condition, typename T = void>
    using EnableIf_t = typename EnableIf<Condition, T>::type;
} // namespace Explain

template <typename T>
auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral_v<T>, bool>
{
    return value > 0 && (value & (value - 1)) == 0;
}

template <typename T>
auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point_v<T>, bool>
{
    int exponent;
    const T mantissa = std::frexp(value, &exponent);
    return mantissa == static_cast<T>(0.5);
}

namespace Alternative
{
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    bool is_power_of_2(T value)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T, typename = void, typename = std::enable_if_t<std::is_floating_point_v<T>>>
    auto is_power_of_2(T value) 
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
} // namespace Alternative

namespace Cpp17
{
    template <typename T>
    bool is_power_of_2(T value)
    {
        if constexpr (std::is_integral_v<T>)
        {
            return value > 0 && (value & (value - 1)) == 0;
        }
        else
        {
            int exponent;
            const T mantissa = std::frexp(value, &exponent);
            return mantissa == static_cast<T>(0.5);
        }
    }
} // namespace Cpp17

namespace Cpp20
{
    template <std::integral T>
    auto is_power_of_2(T value)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <std::floating_point T>
    auto is_power_of_2(T value)
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
} // namespace Cpp20

// bool is_power_of_2(float value)
// {
//     int exponent;
//     const float mantissa = std::frexp(value, &exponent);
//     return mantissa == 0.5f;
// }

TEST_CASE("SFINAE")
{
    REQUIRE(is_power_of_2(64));
    REQUIRE(is_power_of_2(1024L));
    REQUIRE(is_power_of_2(8));
    REQUIRE(!is_power_of_2(65ULL));
    REQUIRE(!is_power_of_2(199));

    REQUIRE(is_power_of_2(8.0));
    REQUIRE(is_power_of_2(64.0));
    REQUIRE(!is_power_of_2(111.0f));
    REQUIRE(is_power_of_2(512.f));
}
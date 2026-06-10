#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include <iostream>

struct UpperCaseFormatter
{
    std::string format(const std::string& message) const
    {
        std::string result = message;
        std::transform(result.begin(), result.end(), 
                       result.begin(), [](char c) { return std::toupper(c); });   
        return result;
    }
};

struct LowerCaseFormatter
{
    std::string format(const std::string& message) const
    {
        std::string result = message;
        std::transform(result.begin(), result.end(), 
                       result.begin(), [](char c) { return std::tolower(c); });   
        return result;
    }
};

struct CapitalizeFormatter
{
    std::string format(const std::string& message) const
    {
        std::string result = message;
        result[0] = std::toupper(result[0]);
        return result;
    }
};

template <typename TFormatter = UpperCaseFormatter>
class Logger 
{
    TFormatter formatter_;
public:   
    Logger() = default;

    Logger(TFormatter formatter)
        : formatter_(std::move(formatter))
    {
    }

    void log(const std::string& message)
    {        
        std::cout << formatter_.format(message) << std::endl;
    }
};

TEST_CASE("static polymorphism")
{
    Logger<CapitalizeFormatter> logger_1;
    logger_1.log("test logger_1");

    Logger<UpperCaseFormatter> logger_2;
    logger_2.log("text logger_2");

    Logger<LowerCaseFormatter> logger_3;
    logger_3.log("TEST LOGGER_3");
}

TEST_CASE("using traits")
{   
    // TODO   
}

TEST_CASE("using policies - Policy Based Design")
{
    // TODO
}
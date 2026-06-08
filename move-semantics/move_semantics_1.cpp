#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace std::literals;

std::string full_name(const std::string& first_name, const std::string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        std::string& lval_ref_name = name;

        const std::string& ref_full_name = full_name(name, std::string("Kowalski"));
        std::cout << ref_full_name << "\n";
        //ref_full_name[0] = 'J'; // compiler-error
    }

    SECTION("C++11")
    {
        std::string&& rval_ref_full_name = full_name(name, "Kowalski");
        rval_ref_full_name[0] = 'J';
        std::cout << rval_ref_full_name << "\n";

        //std::string&& rval_name = name; // ILLEGAL IN C++ - cannot bind an lvalue to rvalue ref
    }
}
#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// Data - class with copy & move semantics (user provided implementation)

using namespace Helpers;

class Data
{
    std::string name_;
    int* data_;
    size_t size_;

public:
    using iterator = int*;
    using const_iterator = const int*;

    Data(std::string name, std::initializer_list<int> list)
        : name_{std::move(name)}
        , size_{list.size()}
    {
        data_ = new int[list.size()];
        std::copy(list.begin(), list.end(), data_);

        std::cout << "Data(" << name_ << ")\n";
    }

    Data(const Data& other)
        : name_(other.name_)
        , size_(other.size_)
    {
        std::cout << "Data(" << name_ << ": cc)\n";
        data_ = new int[size_];
        std::copy(other.begin(), other.end(), data_);
    }

    Data& operator=(const Data& other)
    {
        // if (this != &other)
        // {
        //     delete[] data_;

        //     name_ = other.name_;
        //     size_ = other.size_;
        //     data_ = new int[other.size_];
        //     std::copy(other.begin(), other.end(), data_);
        // }

        // copy-swap idiom
        Data temp(other);
        swap(temp);

        std::cout << "Data=(" << name_ << ": cc)\n";

        return *this;
    }

    // TODO: move semantics
    Data(Data&& other)
        : name_(std::move(other.name_))
        , data_(other.data_)
        , size_(other.size_)
    {
        other.data_ = nullptr;
        other.size_ = 0;

        std::cout << "Data=(" << name_ << ": mv)\n";
    }

    Data& operator=(Data&& other)
    {
        if (this != &other)
        {
            Data temp = std::move(other);
            swap(temp);

            std::cout << "Data=(" << name_ << ": mv)\n";
        }

        return *this;
    }

    ~Data()
    {
        delete[] data_;
    }

    void swap(Data& other)
    {
        name_.swap(other.name_);
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    iterator begin()
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    const_iterator end() const
    {
        return data_ + size_;
    }

    size_t size() const
    {
        return size_;
    }
};

Data create_data_set(const std::string& name = "data-set")
{
    Data ds{name, {54, 6, 34, 235, 64356, 235, 23}};

    return ds;
}

namespace LegacyCode
{
    std::vector<int>* create_large_array(size_t size)
    {
        std::vector<int>* ptr_vec = new std::vector<int>(size);

        return ptr_vec;
    }
} // namespace LegacyCode

namespace ModernCpp
{
    std::vector<int> create_large_array(size_t size)
    {
        std::vector<int> vec(size);

        return vec;
    }
}

TEST_CASE("std::move")
{
    std::vector<int> vec = {1, 2, 3};
    std::vector<int> target = std::move(vec);
    // std::vector<int> target = static_cast<std::vector<int>&&>(vec);

    REQUIRE(target == std::vector<int>{1, 2, 3});
    REQUIRE(vec.size() == 0);

    int number = 42;
    int target_number = std::move(number); // copy of primitive value

    int* ptr_n = &target_number;
    int* ptr_target_n = std::move(ptr_n); // copy of primitive value
}

TEST_CASE("Data & move semantics")
{
    Data ds1{"ds1", {1, 2, 3, 4, 5}};

    Data backup = ds1; // copy
    Helpers::print(backup, "backup");

    Data ds2 = std::move(ds1);
    Helpers::print(ds2, "ds2");

    REQUIRE(ds1.size() == 0);

    backup = std::move(ds2);
    Helpers::print(backup, "backup");

    Data ds3 = create_data_set(); // rvalue returned from function

    std::vector<Data> dataset;
    dataset.reserve(100);

    dataset.push_back(create_data_set("data-1")); // mv
    dataset.push_back(create_data_set("data-2")); // mv
    dataset.push_back(ds3); //cc
    dataset.push_back(std::move(ds3)); // mv
}

TEST_CASE("Legacy vs Modern C++")
{
    SECTION("Legacy")
    {
        std::vector<int>* vec = LegacyCode::create_large_array(1'000'000);
        (*vec)[2] = 53;

        delete vec;
    }

    SECTION("Modern")
    {
        std::vector<int> vec = ModernCpp::create_large_array(10'000'000);
        vec[2] = 665;
    }
}

class DataRow
{
    Data row_1_;
    Data row_2_;
public:
    // DataRow(Data&& row_1, Data&& row_2) : row_1_(std::move(row_1)), row_2_(std::move(row_2))
    // {}

    // DataRow(const Data& row_1, const Data& row_2) : row_1_(row_1), row_2_(row_2)
    // {}

    // DataRow(const Data& row_1, Data&& row_2) : row_1_(row_1), row_2_(std::move(row_2))
    // {}

    // DataRow(Data&& row_1, const Data& row_2) : row_1_(std::move(row_1)), row_2_(row_2)
    // {}

    DataRow(Data row_1, Data row_2) : row_1_(std::move(row_1)), row_2_(std::move(row_2))
    {}

    // Rule of Five
    // DataRow(const DataRow&) = default;
    // DataRow& operator=(const DataRow&) = default;

    // DataRow(DataRow&&) = default;
    // DataRow& operator=(DataRow&&) = default;

    // ~DataRow() 
    // {
    //     std::cout << "~DataRow()\n";
    // }

    // Rule of Zero!!!

    Data& operator[](size_t index)
    {
        if (index == 0)
            return row_1_;
        else if (index == 1)
            return row_2_;
        
        throw std::out_of_range("Index out of range");
    }

    void print() const
    {
        Helpers::print(row_1_, "r1");
        Helpers::print(row_2_, "r2");
    }
};

TEST_CASE("DataRow")
{
    DataRow dr1(create_data_set("row-1"), create_data_set("row-2"));
    dr1.print();

    Data ds = create_data_set("row-3");

    DataRow dr2(ds, create_data_set("row-4"));
    dr2.print();

    std::cout << "------\n";

    DataRow target1 = std::move(dr1);
    target1.print();

    target1 = std::move(dr2);
    target1.print();
}
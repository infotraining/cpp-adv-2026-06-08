#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

namespace Explain
{
    template <typename T>
    class unique_ptr
    {
    public:
        unique_ptr() = default;

        unique_ptr(T* ptr)
        {
            ptr_ = ptr;
        }

        // copy constructor
        unique_ptr(const unique_ptr& other) = delete;

        // copy assignment operator
        unique_ptr& operator=(const unique_ptr& other) = delete;

        // move constructor
        unique_ptr(unique_ptr&& other)
            : ptr_(other.ptr_)
        {
            std::cout << "Move constructor of unique_ptr\n";
            other.ptr_ = nullptr;
        }

        // move assignment
        unique_ptr& operator=(unique_ptr&& other)
        {
            std::cout << "Move assignment of unique_ptr\n";
            if (this != &other) // check if not self-assignment
            {
                delete ptr_;
                ptr_ = other.ptr_; 
                other.ptr_ = nullptr;
            }

            return *this;
        }
        
        ~unique_ptr()
        {
            delete ptr_;
        }

        operator bool() const
        {
            return ptr_ != nullptr;    
        }

        T& operator*() const
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return ptr_;
        }

        T* get() const
        {
            return ptr_;
        }
    private:
        T* ptr_ = nullptr;
    };
} // namespace Explain

TEST_CASE("move semantics - unique_ptr")
{
    {
        std::string* ptr_1 = new std::string("text");

        if (ptr_1)
            std::cout << *ptr_1 << " has length " << ptr_1->size() << "\n";

        delete ptr_1;
    }    

    {
        Explain::unique_ptr<std::string> ptr_1(new std::string("text"));

        if (ptr_1)
            std::cout << *ptr_1 << " has length " << ptr_1->size() << "\n";

        Explain::unique_ptr<std::string> ptr_2 = std::move(ptr_1);
        std::cout << *ptr_2 << "\n";

        ptr_1 = Explain::unique_ptr(new std::string("another text"));
        ptr_1 = std::move(ptr_2);

        ptr_2 = std::move(ptr_2); // self-assignment
    }
}

TEST_CASE("move semantics - unique_ptr - Gadget")
{
    using Helpers::Gadget;

    {
        Gadget* ptr_g = new Gadget(1, "ipad");

        if (ptr_g)
            ptr_g->use();

        delete ptr_g;
    }

    {
        Explain::unique_ptr<Gadget> ptr_g(new Gadget(2, "smartwatch"));

        if (ptr_g)
            ptr_g->use();
    }

    Explain::unique_ptr<Gadget> ptr_g2;
    REQUIRE(ptr_g2.get() == nullptr);
}
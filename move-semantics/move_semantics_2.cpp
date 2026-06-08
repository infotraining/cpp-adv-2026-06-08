#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

namespace Explain
{
    class unique_ptr
    {
    public:
        unique_ptr(std::string* text_ptr)
        {
            text_ptr_ = text_ptr;
        }

        // copy constructor
        unique_ptr(const unique_ptr& other) = delete;

        // copy assignment operator
        unique_ptr& operator=(const unique_ptr& other) = delete;

        // move constructor
        unique_ptr(unique_ptr&& other)
        {
            std::cout << "Move constructor of unique_ptr\n";
            text_ptr_ = other.text_ptr_;
            other.text_ptr_ = nullptr;
        }

        // move assignment
        unique_ptr& operator=(unique_ptr&& other)
        {
            std::cout << "Move assignment of unique_ptr\n";
            if (this != &other) // check if not self-assignment
            {
                delete text_ptr_;
                text_ptr_ = other.text_ptr_; 
                other.text_ptr_ = nullptr;
            }

            return *this;
        }
        
        ~unique_ptr()
        {
            delete text_ptr_;
        }

        operator bool() const
        {
            return text_ptr_ != nullptr;    
        }

        std::string& operator*() const
        {
            return *text_ptr_;
        }

        std::string* operator->() const
        {
            return text_ptr_;
        }
    private:
        std::string* text_ptr_;
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
        Explain::unique_ptr ptr_1(new std::string("text"));

        if (ptr_1)
            std::cout << *ptr_1 << " has length " << ptr_1->size() << "\n";

        Explain::unique_ptr ptr_2 = std::move(ptr_1);
        std::cout << *ptr_2 << "\n";

        ptr_1 = Explain::unique_ptr(new std::string("another text"));
        ptr_1 = std::move(ptr_2);

        ptr_2 = std::move(ptr_2); // self-assignment
    }
}
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <span>
#include <catch2/catch_test_macros.hpp>

using namespace std;

#define UNSAFE 0

class Gadget
{
public:
    Gadget(int id = 0)
        : id_{id}
    {
        std::cout << "Constructing Gadget(" << id_ << ")\n";
    }

    Gadget(const Gadget&) = delete;
    Gadget& operator=(const Gadget&) = delete;

    ~Gadget()
    {
        std::cout << "Destroying ~Gadget(" << id_ << ")\n";
    }

    int id() const
    {
        return id_;
    }

    void set_id(int id)
    {
        id_ = id;
    }

    void use()
    {
        std::cout << "Using a gadget with id: " << id() << '\n';
    }

    void unsafe_might_throw()
    {
        std::cout << "Using a gadget with id: " << id() << " - Ups... It crashed..." << std::endl;
        if  (UNSAFE)
            throw std::runtime_error("ERROR");
    }

private:
    int id_;
};

// This code is part of the legacy code and should not be modified
namespace LegacyCode
{
    Gadget* create_many_gadgets(unsigned int size)
    {
        Gadget* many_gadgets = new Gadget[size];

        for (unsigned int i = 0; i < size; ++i)
            many_gadgets[i].set_id(i);

        return many_gadgets;
    }
}

void reset_value(Gadget& g, int n)
{
    // some logic

    g.set_id(n);
    cout << "New id for Gadget: " << g.id() << endl;
}

//////////////////////////////////////////////
// TODO - modernize the code below

std::unique_ptr<Gadget> create_gadget(int arg)
{
    return std::unique_ptr<Gadget>(new Gadget(arg));
}

class Player
{
    std::unique_ptr<Gadget> gadget_;
    std::ostream* logger_;

    Player(const Player&);
    Player& operator=(const Player&);

public:
    Player(std::unique_ptr<Gadget> g, std::ostream* logger = NULL)
        : gadget_(std::move(g))
        , logger_(logger)
    {
        if (gadget_ == nullptr)
            throw std::invalid_argument("Gadget can not be null");
    }

    ~Player()
    {
        if (logger_)
            *logger_ << "Destroing a gadget: " << gadget_->id() << std::endl;
    }

    void play()
    {
        if (logger_)
            *logger_ << "Player is using a gadget: " << gadget_->id() << std::endl;

        gadget_->use();
    }
};

TEST_CASE("Exercise 1 - Modernize the code using smart pointers")
{
    std::unique_ptr<Gadget> ptr_gadget = create_gadget(4);

    /* kod korzystajacy z ptrX */

    reset_value(*ptr_gadget, 5);

    ptr_gadget->unsafe_might_throw();
}

TEST_CASE("Exercise 2 - Modernize the code using smart pointers")
{
    const int size = 10;

    std::unique_ptr<Gadget[]> buffer(LegacyCode::create_many_gadgets(size));

    // since C++20
    std::span<Gadget> safe_array{buffer.get(), size};
    for(auto& g : safe_array)
    {
        g.unsafe_might_throw();
    }

    for (int i = 0; i < size; ++i)
        buffer[i].unsafe_might_throw();
}

TEST_CASE("Exercise 3 - Modernize the code using smart pointers")
{
    vector<std::unique_ptr<Gadget>> my_gadgets;

    my_gadgets.push_back(create_gadget(87));
    my_gadgets.push_back(create_gadget(12));
    my_gadgets.push_back(std::make_unique<Gadget>(98));

    int value_generator = 0;
    for (vector<std::unique_ptr<Gadget>>::iterator it = my_gadgets.begin(); it != my_gadgets.end(); ++it)
    {
        cout << "Gadget's old id: " << (*it)->id() << endl;
        reset_value(**it, ++value_generator);
    }

    std::cout << "--------\n";

    my_gadgets[0].reset();
    my_gadgets.erase(my_gadgets.begin());

    std::cout << "--------\n";

    Player p(std::move(my_gadgets.back()));
    my_gadgets.pop_back();
    p.play();

    my_gadgets[0]->unsafe_might_throw();
}
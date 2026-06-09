#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <catch2/catch_test_macros.hpp>

class Observer
{
public:
    virtual void update(const std::string& event_args) = 0;
    virtual ~Observer() = default;
};

class Subject
{
    int state_;
    std::set<std::weak_ptr<Observer>, std::owner_less<std::weak_ptr<Observer>>> observers_;

public:
    Subject()
        : state_(0)
    {
    }

    void register_observer(std::weak_ptr<Observer> observer)
    {
        observers_.insert(observer);
    }

    void unregister_observer(std::weak_ptr<Observer> observer)
    {
        observers_.erase(observer);
    }

    void set_state(int new_state)
    {
        if (state_ != new_state)
        {
            state_ = new_state;
            notify("Changed state on: " + std::to_string(state_));
        }
    }

protected:
    void notify(const std::string& event_args)
    {
        for (auto it = observers_.begin(); it != observers_.end();)
        {
            std::shared_ptr<Observer> living_ptr = it->lock(); 

            if (living_ptr)
            {
                living_ptr->update(event_args);
                ++it;
            }
            else
            {
                it = observers_.erase(it);
            }
        }
    }
};

class Logger : public Observer
{
public:
    void update(const std::string& event) override
    {
        std::cout << "Logging - event: " << event << std::endl;
    }
};

class Device : public Observer
{
public:
    void update(const std::string& event) override
    {
        std::cout << "Device notified about event: " << event << std::endl;
    }
};

TEST_CASE("using observer pattern")
{
    using namespace std;

    Subject s;

    std::shared_ptr<Logger> logger = std::make_shared<Logger>();
    s.register_observer(logger);

    {
        std::shared_ptr<Device> device = std::make_shared<Device>();
        s.register_observer(device);

        s.set_state(1);

        cout << "End of scope." << endl;
    }

    s.set_state(2);
}

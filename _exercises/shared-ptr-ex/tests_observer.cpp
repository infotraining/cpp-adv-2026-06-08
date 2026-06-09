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
    std::set<Observer*> observers_;

public:
    Subject() : state_(0)
    {
    }

    void register_observer(Observer* observer)
    {
        observers_.insert(observer);
    }

    void unregister_observer(Observer* observer)
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
        for (Observer* observer : observers_)
        {
            observer->update(event_args);
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

    Logger* logger = new Logger;
    s.register_observer(logger);

    {
        Device* device = new Device;
        s.register_observer(device);

        s.set_state(1);

        delete device;

        cout << "End of scope." << endl;
    }

    s.set_state(2);
}

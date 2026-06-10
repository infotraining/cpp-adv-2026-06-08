#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <deque>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using namespace Helpers;

////////////////////////////////////////////////////////
///  PERFECT FORWARDING
void have_fun(Gadget& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}

void have_fun(const Gadget& cg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    cg.use();
}

void have_fun(Gadget&& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}


namespace WithoutPerfectForwarding
{
    void use(Gadget& g)
    {
        have_fun(g); // have_fun(Gadget& g)
    }

    void use(const Gadget& g)
    {
        have_fun(g); // have_fun(const Gadget& cg)
    }

    void use(Gadget&& g)
    {
        have_fun(std::move(g)); // have_fun(Gadget&& g)
    }

    void use_both(Gadget& g1, Gadget& g2)
    {
        have_fun(g1);
        have_fun(g2);
    }

    void use_both(const Gadget& g1, Gadget& g2)
    {
        have_fun(g1);
        have_fun(g2);
    }

    void use_both(Gadget& g1, const Gadget& g2)
    {
        have_fun(g1);
        have_fun(g2);
    }

    void use_both(const Gadget& g1, const Gadget& g2)
    {
        have_fun(g1);
        have_fun(g2);
    }
    
    void use_both(Gadget&& g1, Gadget&& g2)
    {
        have_fun(std::move(g1));
        have_fun(std::move(g2));
    }

    // ...
} // namespace WithoutPerfectForwarding

namespace PerfectForwarding
{
    template <typename T>
    void use(T&& g)
    {
        // // if g was lvalue -> T is lvalue ref type
        // have_fun(g);
        // // else
        // have_fun(std::move(g));
        have_fun(std::forward<T>(g));
    }

    template <typename T1, typename T2>
    void use_both(T1&& g1, T2&& g2)
    {
        have_fun(std::forward<T1>(g1));
        have_fun(std::forward<T2>(g2));
    }
}

TEST_CASE("perfect forwarding - using gadget")
{
    Gadget g{1, "g"};
    const Gadget cg{2, "const g"};

    using WithoutPerfectForwarding::use;

    use(g);
    use(cg);
    use(Gadget{3, "temporary gadget"});
}

TEST_CASE("emplace")
{
    std::vector<Gadget> gadgets;

    gadgets.push_back(Gadget(1, "ipad"));
    gadgets.emplace_back(2, "ipod");
}
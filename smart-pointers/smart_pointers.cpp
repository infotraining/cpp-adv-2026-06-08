#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

using Helpers::Gadget;

// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

namespace LegacyCode
{
    // forward declarations
    Gadget* get_gadget(const std::string& name);
    Gadget* get_gadget_alternative();
    void use(Gadget* g);
    void use_gadget(Gadget* g);
    void free_gadget(Gadget* g);

    // definitions
    Gadget* get_gadget(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
    }

    Gadget* get_gadget_alternative()
    {
        static Gadget g{42, "global-gadget"};
        return &g;
    }

    void use(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void free_gadget(Gadget* g)
    {
        delete g;
    }
} // namespace LegacyCode

TEST_CASE("legacy hell with dynamic memory")
{
    using namespace LegacyCode;

    {
        Gadget* g = get_gadget("ipad");

        use_gadget(g);
    } // memory leak

    {
        use_gadget(get_gadget("ipad"));
    } // memory leak

    {
        Gadget* g = new Gadget(13, "ipad");

        use(g);
        // use_gadget(g);  // UB!!! - use after delete
        // std::cout << g->name() << std::endl; // UB!!! - use after delete
    }

    {
        Gadget* g = get_gadget("ipad");

        use(g);

        // delete g; // UB!!! - second delete
    }

    {
        Gadget* g = get_gadget_alternative();

        delete g; // UB! delete on static object
    }
}

namespace ModernCpp
{
    // forward declarations
    std::unique_ptr<Gadget> get_gadget(const std::string& name);
    Gadget* get_gadget_alternative();
    void use(std::unique_ptr<Gadget> g);
    void use_gadget(Gadget* g);

    // definitions
    std::unique_ptr<Gadget> get_gadget(const std::string& name)
    {
        static int id = 665;
        return std::unique_ptr<Gadget>(new Gadget(++id, name));
    }

    Gadget* get_gadget_alternative()
    {
        static Gadget g{42, "global-gadget"};
        return &g;
    }

    void use(std::unique_ptr<Gadget> g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
} // namespace ModernCpp

TEST_CASE("modern memory managment")
{
    using namespace ModernCpp;

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use_gadget(g.get());
    }

    {
        use(get_gadget("ipad"));
    }

    {
        std::unique_ptr<Gadget> g = std::make_unique<Gadget>(42, "ipad");

        use(std::move(g));
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use(std::move(g));
    }

    {
        Gadget* g = get_gadget_alternative();
    }

    {
        std::unique_ptr<Gadget> g(LegacyCode::get_gadget("ipad"));

        g->use();

        LegacyCode::free_gadget(g.release());
    }
}

class User
{
    std::unique_ptr<Gadget> g_;

public:
    explicit User(std::unique_ptr<Gadget> g)
        : g_{std::move(g)}
    { }

    void use_gadget()
    {
        if (g_)
            g_->use();
    }
};

TEST_CASE("using unique_ptr as class member")
{
    User u1(std::make_unique<Gadget>(665, "ipad"));
    u1.use_gadget();

    User u2 = std::move(u1);
    u2.use_gadget();
}

class Shape
{
public:
    virtual void draw() const = 0;
    virtual ~Shape() = default;
};

struct Circle : public Shape
{
    int r;

    Circle(int radius)
        : r{radius}
    { }

    void draw() const override
    {
        std::cout << "Drawing Circle with radius: " << r << "\n";
    }
};

struct Rectangle : public Shape
{
    int w;
    int h;

    Rectangle(int width, int height)
        : w{width}
        , h{height}
    { }

    void draw() const override
    {
        std::cout << "Drawing Rectangle with width: " << w << " and height: " << h << "\n";
    }
};

TEST_CASE("smart pointers & polymorphism")
{
    std::unique_ptr<Shape> shape;

    shape = std::make_unique<Circle>(5);
    shape->draw();

    shape = std::make_unique<Rectangle>(10, 20);
    shape->draw();

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(3));
    shapes.push_back(std::make_unique<Rectangle>(4, 5));
    shapes.push_back(std::move(shape));

    for (const auto& s : shapes)
        s->draw();
}

void may_throw()
{
    // throw std::runtime_error("Error#13");
}

class Socket
{
public:
    Socket(int port)
    { }

    void write(const std::string& msg) { }

    void flush() { std::cout << "Socket flushed...\n"; }
    void close() { std::cout << "Socket closed...\n"; }
};

TEST_CASE("Custom deallocators")
{
    SECTION("Legacy code")
    {
        FILE* f = fopen("text.txt", "w+");

        if (!f)
        {
            std::cout << "File not opened!!!\n";
            throw std::runtime_error("File not opened!!!");
        }

        fprintf(f, "abc");
        may_throw();

        fclose(f); // explicit close file
    }

    SECTION("Modern C++")
    {
        using DeallocType = int (*)(FILE*);

        std::unique_ptr<FILE, DeallocType> safe_f(fopen("text_safe.txt", "w+"), &fclose);

        if (!safe_f)
        {
            std::cout << "File not opened!!!\n";
            throw std::runtime_error("File not opened!!!");
        }

        fprintf(safe_f.get(), "abc");
        may_throw();
    }

    SECTION("Custom deallocator - lambda")
    {
        Socket socket{8080};

        {
            auto socket_closer = [](Socket* s) {
                s->flush();
                s->close();
            };

            std::unique_ptr<Socket, decltype(socket_closer)> s_guard(&socket, socket_closer);

            s_guard->write("Hello");
        }
    }
}

namespace LegacyCode
{
    int* create_array()
    {
        int* array_1 = new int[1024];

        array_1[0] = 42;

        return array_1;
    }
}

TEST_CASE("unique_ptr with array types")
{
    SECTION("Legacy code")
    {
        int* array_1 = new int[1024];

        array_1[0] = 42;

        delete[] array_1;
    }

    SECTION("ModernCpp")
    {
        std::unique_ptr<int[]> array_1(LegacyCode::create_array());

        array_1[0] = 42;
    }
}
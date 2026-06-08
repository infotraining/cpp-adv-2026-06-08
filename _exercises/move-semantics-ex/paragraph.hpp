#ifndef PARAGRAPH_HPP_
#define PARAGRAPH_HPP_

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

namespace LegacyCode
{
    // TODO - implement move semantics for Paragraph
    class Paragraph
    {
        char* buffer_;

    protected:
        void swap(Paragraph& p) noexcept
        {
            std::swap(buffer_, p.buffer_);
        }

    public:
        Paragraph()
            : buffer_(new char[1024])
        {
            std::strcpy(buffer_, "Default text!");
        }

        Paragraph(const char* txt)
            : buffer_(new char[1024])
        {
            std::strcpy(buffer_, txt);
        }

        Paragraph(const Paragraph& p)
            : buffer_(new char[1024])
        {
            std::strcpy(buffer_, p.buffer_);
        }

        Paragraph& operator=(const Paragraph& p)
        {
            Paragraph temp(p);
            swap(temp);

            return *this;
        }

        // move constructor
        Paragraph(Paragraph&& other) noexcept
            : buffer_(other.buffer_)
        {
            other.buffer_ = nullptr;
        }

        // move assignment constructor
        Paragraph& operator=(Paragraph&& other) noexcept
        {
            if (this != &other)
            {
                delete[] buffer_;
                buffer_ = other.buffer_;
                other.buffer_ = nullptr;
            }

            return *this;
        }

        void set_paragraph(const char* txt)
        {
            std::strcpy(buffer_, txt);
        }

        const char* get_paragraph() const noexcept
        {
            return buffer_;
        }

        void render_at(int posx, int posy) const
        {
            std::cout << "Rendering text '" << buffer_ << "' at: [" << posx << ", " << posy << "]" << std::endl;
        }

        ~Paragraph() noexcept
        {
            delete[] buffer_;
        }
    };
} // namespace LegacyCode

class Shape
{
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

// TODO - ensure that Text is copyable & moveable type
class Text : public Shape
{
    int x_, y_;
    LegacyCode::Paragraph p_;

public:
    Text(int x, int y, const std::string& text)
        : x_{x}
        , y_{y}
        , p_{text.c_str()}
    {
    }

    void draw() const override
    {
        p_.render_at(x_, y_);
    }

    std::string text() const
    {
        const char* txt = p_.get_paragraph();
        return (txt == nullptr) ? "" : txt;
    }

    void set_text(const std::string& text)
    {
        p_.set_paragraph(text.c_str());
    }
};

struct ShapeGroup : public Shape
{
    std::vector<std::unique_ptr<Shape>> shapes;

    ShapeGroup() = default;

    void draw() const override
    {
        for (const auto& s : shapes)
            s->draw();
    }

    // TODO - implement adding a shape to a shapes container

    void add(std::unique_ptr<Shape> ptr)
    {
        shapes.push_back(std::move(ptr));
    }
};

#endif /*PARAGRAPH_HPP_*/

#ifndef SRC_PREDICATES_HPP
#define SRC_PREDICATES_HPP

#include <string>
#include <cctype>

struct CharPred
{
    const std::string name;

    CharPred(const std::string& n) :
        name(n) {}

    virtual bool operator()(const char c) const = 0;
};

inline struct IsSpace : public CharPred
{
    IsSpace() :
        CharPred("space") {}
    virtual bool operator()(const char c) const override
    {
        return std::isspace(c) != 0;
    }
} IsSpace;

inline struct IsInlineSpace : public CharPred
{
    IsInlineSpace() :
        CharPred("inline space") {}
    virtual bool operator()(const char c) const override
    {
        return (std::isspace(c) != 0) && (c != '\n') && (c != '\r');
    }
} IsInlineSpace;

inline struct IsDigit : public CharPred
{
    IsDigit() :
        CharPred("digit") {}
    virtual bool operator()(const char c) const override
    {
        return std::isdigit(c) != 0;
    }
} IsDigit;

inline struct IsUpper : public CharPred
{
    IsUpper() :
        CharPred("uppercase") {}
    virtual bool operator()(const char c) const override
    {
        return std::isupper(c) != 0;
    }
} IsUpper;

inline struct IsLower : public CharPred
{
    IsLower() :
        CharPred("lowercase") {}
    virtual bool operator()(const char c) const override
    {
        return std::islower(c) != 0;
    }
} IsLower;

inline struct IsAlpha : public CharPred
{
    IsAlpha() :
        CharPred("alphabetic") {}
    virtual bool operator()(const char c) const override
    {
        return std::isalpha(c) != 0;
    }
} IsAlpha;

inline struct IsAlnum : public CharPred
{
    IsAlnum() :
        CharPred("alphanumeric") {}
    virtual bool operator()(const char c) const override
    {
        return std::isalnum(c) != 0;
    }
} IsAlnum;

inline struct IsPrint : public CharPred
{
    IsPrint() :
        CharPred("printable") {}
    virtual bool operator()(const char c) const override
    {
        return std::isprint(c) != 0;
    }
} IsPrint;

struct IsChar : public CharPred
{
    explicit IsChar(const char c) :
        CharPred("'" + std::string(1, c) + "'"), m_k(c)
    {}
    virtual bool operator()(const char c) const override
    {
        return m_k == c;
    }

private:
    const char m_k;
};

struct IsEither : public CharPred
{
    explicit IsEither(const CharPred& a, const CharPred& b) :
        CharPred("(" + a.name + " | " + b.name + ")"), m_a(a), m_b(b)
    {}
    virtual bool operator()(const char c) const override
    {
        return m_a(c) || m_b(c);
    }

private:
    const CharPred& m_a;
    const CharPred& m_b;
};

struct IsNot : public CharPred
{
    explicit IsNot(const CharPred& a) :
        CharPred("~" + a.name), m_a(a)
    {}
    virtual bool operator()(const char c) const override
    {
        return !m_a(c);
    }

private:
    const CharPred& m_a;
};

inline struct IsSymbol : public CharPred
{
    IsSymbol() :
        CharPred("sym") {}
    virtual bool operator()(const char c) const override
    {
        switch (c)
        {
            case ':':
            case '!':
            case '?':
            case '@':
            case '_':
            case '-':
            case '+':
            case '*':
            case '/':
            case '|':
            case '=':
            case '<':
            case '>':
            case '%':
            case '$':
                return true;

            default:
                return false;
        }
    }
} IsSymbol;

inline struct IsAny : public CharPred
{
    IsAny() :
        CharPred("any") {}
    virtual bool operator()(const char) const override
    {
        return true;
    }
} IsAny;

const IsChar IsMinus('-');

#endif

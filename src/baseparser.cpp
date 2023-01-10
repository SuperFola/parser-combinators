#include "baseparser.hpp"

#include <iostream>

BaseParser::BaseParser(const std::string& s) :
    backtrack_count(0), m_str(s), m_row(0), m_col(0)
{
    // if the input string is empty, raise an error
    if (s.size() == 0)
    {
        m_sym = utf8_char_t();
        error("Expected symbol, got empty string", "");
    }

    m_it = m_next_it = m_str.begin();

    // otherwise, get the first symbol
    next();
}

void BaseParser::next()
{
    m_it = m_next_it;

    // getting a character from the stream
    auto [it, sym] = utf8_char_t::at(m_it);
    m_next_it = it;
    m_sym = sym;

    // TODO reimplement BaseParser::next()
    // char previous_sym = m_count > 0 ? m_str[m_count - 1] : 0;
    // if (previous_sym == '\n')
    // {
    //     ++m_row;
    //     m_col = 0;
    // }
    // else if (0 <= m_sym && std::isprint(previous_sym))
    //     ++m_col;
}

void BaseParser::backtrack(long n)
{
    backtrack_count++;

    m_it = m_str.begin() + n;
    auto [it, sym] = utf8_char_t::at(m_it);
    m_next_it = it;
    m_sym = sym;

    // TODO reimplement BaseParser::backtrack()
    // m_row = 0;
    // m_col = 0;
    // // adjust the row/col count (this is going to be VERY inefficient)
    // for (std::size_t i = 0; i < n; ++i)
    // {
    //     if (m_str[i] == '\n')
    //     {
    //         ++m_row;
    //         m_col = 0;
    //     }
    //     else if (0 <= m_str[i] && std::isprint(m_str[i]))
    //         ++m_col;
    // }
}

bool BaseParser::accept(const CharPred& t, std::string* s)
{
    if (isEOF())
        return false;

    // return false if the predicate couldn't consume the symbol
    if (!t(m_sym.codepoint()))
        return false;
    // otherwise, add it to the string and go to the next symbol
    if (s != nullptr)
        *s += m_sym.c_str();

    next();
    return true;
}

bool BaseParser::expect(const CharPred& t, std::string* s)
{
    // throw an error if the predicate couldn't consume the symbol
    if (!t(m_sym.codepoint()))
        error("Expected " + t.name, m_sym.c_str());
    // otherwise, add it to the string and go to the next symbol
    if (s != nullptr)
        *s += m_sym.c_str();
    next();
    return true;
}

bool BaseParser::space(std::string* s)
{
    if (accept(IsSpace))
    {
        if (s != nullptr)
            s->push_back(' ');
        // loop while there are still ' ' to consume
        while (accept(IsSpace))
            ;
        return true;
    }
    return false;
}

bool BaseParser::inlineSpace(std::string* s)
{
    if (accept(IsInlineSpace))
    {
        if (s != nullptr)
            s->push_back(' ');
        // loop while there are still ' ' to consume
        while (accept(IsInlineSpace))
            ;
        return true;
    }
    return false;
}

bool BaseParser::endOfLine(std::string* s)
{
    if ((accept(IsChar('\r')) || true) && accept(IsChar('\n')))
    {
        if (s != nullptr)
            s->push_back('\n');
        while ((accept(IsChar('\r')) || true) && accept(IsChar('\n')))
            ;
        return true;
    }
    return false;
}

bool BaseParser::number(std::string* s)
{
    if (accept(IsDigit, s))
    {
        // consume all the digits available,
        // stop when the symbole isn't a digit anymore
        while (accept(IsDigit, s))
            ;
        return true;
    }
    return false;
}

bool BaseParser::signedNumber(std::string* s)
{
    return accept(IsMinus, s), number(s);
}

bool BaseParser::name(std::string* s)
{
    auto alnum_symbols = IsEither(IsAlnum, IsSymbol);

    if (accept(alnum_symbols, s))
    {
        while (accept(alnum_symbols, s))
            ;
        return true;
    }
    return false;
}

bool BaseParser::packageName(std::string* s)
{
    if (accept(IsAlnum, s))
    {
        while (accept(IsEither(IsAlnum, IsChar('_')), s))
            ;
        return true;
    }
    return false;
}

bool BaseParser::anyUntil(const CharPred& delim, std::string* s)
{
    if (accept(IsNot(delim), s))
    {
        while (accept(IsNot(delim), s))
            ;
        return true;
    }
    return false;
}

bool BaseParser::oneOf(std::initializer_list<std::string> words, std::string* s)
{
    std::string buffer;
    if (!name(&buffer))
        return false;

    if (s)
        *s = buffer;

    for (auto word : words)
    {
        if (word == buffer)
            return true;
    }
    return false;
}

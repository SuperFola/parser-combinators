#ifndef SRC_BASEPARSER_HPP
#define SRC_BASEPARSER_HPP

#include <string>
#include <exception>
#include <stdexcept>
#include <utility>
#include <initializer_list>

#include "predicates.hpp"
#include "utf8_char.hpp"

struct ParseError : public std::runtime_error
{
    const std::size_t line;
    const std::size_t col;
    const std::string expr;
    const utf8_char_t symbol;

    ParseError(const std::string& what, std::size_t lineNum, std::size_t column, std::string exp, utf8_char_t sym) :
        std::runtime_error(what), line(lineNum), col(column), expr(std::move(exp)), symbol(sym)
    {}
};

class BaseParser
{
public:
    BaseParser(const std::string& s);

    // for debug purpose
    unsigned long backtrack_count;

private:
    std::string m_str;
    std::string::iterator m_it, m_next_it;
    std::size_t m_row;
    std::size_t m_col;
    utf8_char_t m_sym;

    /*
        getting next character and changing the values of count/row/col/sym
    */
    void next();

protected:
    // TODO implement more error() helper functions (eg handle backtracking automatically?)
    inline void error(const std::string& error, const std::string exp)
    {
        throw ParseError(error, m_row, m_col, exp, m_sym);
    }

    // basic getters
    inline std::size_t getCol() { return m_col; }
    inline std::size_t getRow() { return m_row; }
    inline long getCount() { return std::distance(m_str.begin(), m_it); }
    inline std::size_t getSize() { return m_str.size(); }
    inline bool isEOF() { return m_it == m_str.end(); }

    void backtrack(long n);

    /*
        Function to use and check if a Character Predicate was able to parse
        the current symbol.
        Add the symbol to the given string (if there was one) and call next()
    */
    bool accept(const CharPred& t, std::string* s = nullptr);

    /*
        Function to use and check if a Character Predicate was able to parse
        the current Symbol.
        Add the symbol to the given string (if there was one) and call next().
        Throw a ParseError if it couldn't.
    */
    bool expect(const CharPred& t, std::string* s = nullptr);

    // basic parsers
    bool space(std::string* s = nullptr);
    bool inlineSpace(std::string* s = nullptr);
    bool endOfLine(std::string* s = nullptr);
    bool number(std::string* s = nullptr);
    bool signedNumber(std::string* s = nullptr);
    bool name(std::string* s = nullptr);
    bool packageName(std::string* s = nullptr);
    bool anyUntil(const CharPred& delim, std::string* s = nullptr);

    bool oneOf(std::initializer_list<std::string> words, std::string* s = nullptr);
};

#endif

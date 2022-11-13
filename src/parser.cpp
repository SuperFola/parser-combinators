#include "parser.hpp"
#include "combinator.hpp"

#include <iostream>

Parser::Parser(const std::string& code) :
    m_ast(NodeType::List), ParserCombinators(code)
{}

void Parser::parse()
{
    while (!isEOF())
    {
        // parsing single line comments as instructions
        space();
        if (!isEOF())
            comment();
        else
            break;
        space();

        auto n = node();
        if (n)
            m_ast.push_back(n.value());
    }

    std::cout << m_ast << std::endl;
}

bool Parser::comment()
{
    if (accept(IsChar('#')))
    {
        while (accept(IsNot(IsChar('\n'))))
            ;
        accept(IsChar('\n'));
        return true;
    }
    return false;
}

std::optional<Node> Parser::node()
{
    std::vector<std::function<std::optional<Node>()>> methods = {
        [this]() -> std::optional<Node> {
            return letMutSet();
        },
        [this]() -> std::optional<Node> {
            return del();
        },
        [this]() -> std::optional<Node> {
            return condition();
        },
        //[this]() -> std::optional<Node> { return loop(); },
        //[this]() -> std::optional<Node> { return import_(); },
        //[this]() -> std::optional<Node> { return block(); },
        //[this]() -> std::optional<Node> { return function(); },
        //[this]() -> std::optional<Node> { return macro(); },
    };

    if (!accept(IsChar('(')))
        return std::nullopt;
    space();

    // save current position in buffer to be able to go back if needed
    auto position = getCount();

    for (std::size_t i = 0, end = methods.size(); i < end; ++i)
    {
        auto result = methods[i]();

        if (result.has_value())
        {
            space();
            if (accept(IsChar(')')))
                return result;
            else
                errorWithNextToken("Missing closing paren after node");
        }
        else
            backtrack(position);
    }

    errorWithNextToken("Couldn't parse node");
    return std::nullopt;  // will never reach
}

std::optional<Node> Parser::letMutSet()
{
    std::string keyword;
    if (!name(&keyword))
        return std::nullopt;
    if (keyword != "let" && keyword != "mut" && keyword != "set")
        return std::nullopt;

    space();

    std::string symbol;
    if (!name(&symbol))
        errorWithNextToken(keyword + " needs a symbol");

    space();

    auto value = atom();
    if (!value)
        errorWithNextToken("Expected a value");

    Node leaf(NodeType::List);
    leaf.push_back(Node(NodeType::Keyword, keyword));
    leaf.push_back(Node(NodeType::Symbol, symbol));
    leaf.push_back(value.value());

    return leaf;
}

std::optional<Node> Parser::del()
{
    std::string keyword;
    if (!name(&keyword))
        return std::nullopt;
    if (keyword != "del")
        return std::nullopt;

    space();

    std::string symbol;
    if (!name(&symbol))
        errorWithNextToken(keyword + " needs a symbol");

    Node leaf(NodeType::List);
    leaf.push_back(Node(NodeType::Keyword, keyword));
    leaf.push_back(Node(NodeType::Symbol, symbol));

    return leaf;
}

std::optional<Node> Parser::condition()
{
    std::string keyword;
    if (!name(&keyword))
        return std::nullopt;
    if (keyword != "if")
        return std::nullopt;

    space();

    auto condition = atom();
    if (!condition)
        errorWithNextToken("If need a valid condition");  // TODO handle nodes

    space();

    auto value_if_true = atom();  // TODO handle nodes
    if (!value_if_true)
        errorWithNextToken("Expected a value");

    space();

    auto value_if_false = atom();  // TODO handle nodes
    if (value_if_false)
        space();

    Node leaf(NodeType::List);
    leaf.push_back(Node(NodeType::Keyword, keyword));
    leaf.push_back(condition.value());
    leaf.push_back(value_if_true.value());
    if (value_if_false)
        leaf.push_back(value_if_false.value());

    return leaf;
}

std::optional<Node> Parser::loop()
{
    return std::nullopt;
}

std::optional<Node> Parser::import_()
{
    return std::nullopt;
}

std::optional<Node> Parser::block()
{
    return std::nullopt;
}

std::optional<Node> Parser::function()
{
    return std::nullopt;
}

std::optional<Node> Parser::macro()
{
    return std::nullopt;
}

std::optional<Node> Parser::atom()
{
    std::vector<std::function<std::optional<Node>()>> parsers = {
        // numbers
        [this]() -> std::optional<Node> {
            std::string res;
            if (signedNumber(&res))
                return Node(std::stoi(res));  // FIXME stoi?
            return std::nullopt;
        },
        // strings
        [this]() -> std::optional<Node> {
            std::string res;
            if (accept(IsChar('"')))
            {
                while (accept(IsNot(IsChar('"')), &res))
                    ;
                expect(IsChar('"'));

                return Node(NodeType::String, res);
            }
            return std::nullopt;
        },
        // true/false/nil
        [this]() -> std::optional<Node> {
            std::string res;
            if (!name(&res))
                return std::nullopt;
            if (res == "false" || res == "true" || res == "nil")
                return Node(NodeType::Symbol, res);
            return std::nullopt;
        }
    };

    auto pos = getCount();

    for (auto parser : parsers)
    {
        auto result = parser();
        if (result.has_value())
            return result;
        else
            backtrack(pos);
    }

    return std::nullopt;
}

void Parser::errorWithNextToken(const std::string& message)
{
    std::string next_token;
    anyUntil(IsInlineSpace, &next_token);
    error(message, next_token);
}

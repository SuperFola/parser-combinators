#ifndef PARSER_HPP
#define PARSER_HPP

#include "baseparser.hpp"
#include "node.hpp"

#include <string>
#include <optional>
#include <vector>
#include <functional>

// TODO move all the complexity from Parser to BaseParser
class Parser : public BaseParser
{
public:
    Parser(const std::string& code, bool debug);

    void parse();
    const Node& ast() const;

private:
    Node m_ast;
    bool m_debug;
    std::vector<std::function<std::optional<Node>()>> m_node_parsers;  // TODO stop using std::function
    std::vector<std::function<std::optional<Node>()>> m_atom_parsers;  // TODO stop using std::function

    bool comment();
    bool newlineOrComment();

    std::optional<Node> node();
    std::optional<Node> letMutSet();
    std::optional<Node> del();
    std::optional<Node> condition();
    std::optional<Node> loop();
    std::optional<Node> import_();
    std::optional<Node> block();
    std::optional<Node> function();
    std::optional<Node> macro();
    std::optional<Node> functionCall();
    std::optional<Node> list();

    std::optional<Node> atom();
    std::optional<Node> anyAtomOf(std::initializer_list<NodeType> types);
    std::optional<Node> nodeOrValue();
    std::optional<Node> wrapped(std::optional<Node> (Parser::*parser)(), char prefix, char suffix);  // TODO decompose in wrapped_begin and wrapped_end?

    void errorWithNextToken(const std::string& message);  // TODO move this in the base parser
};

#endif

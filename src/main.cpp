#include "parser.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

inline std::vector<std::string> splitString(const std::string& source, char sep)
{
    std::vector<std::string> output;
    output.emplace_back();

    for (char c : source)
    {
        if (c != sep)
            output.back() += c;
        else
            output.emplace_back();  // add empty string
    }

    return output;
}

void makeContext(std::ostream& os, const std::string& code, std::size_t line, std::size_t col_start, std::string exp)
{
    std::vector<std::string> ctx = splitString(code, '\n');

    // TODO use the variable 'exp' to do multiline hightlighting

    std::size_t col_end = std::min<std::size_t>(col_start + exp.size(), ctx[line].size());
    std::size_t first = line >= 3 ? line - 3 : 0;
    std::size_t last = (line + 3) <= ctx.size() ? line + 3 : ctx.size();

    for (std::size_t loop = first; loop < last; ++loop)
    {
        std::string current_line = ctx[loop];
        os << std::setw(5) << (loop + 1) << " | " << current_line << "\n";

        if (loop == line)
        {
            os << "      | ";

            // padding of spaces
            for (std::size_t i = 0; i < (col_start > 0 ? col_start - 1 : col_start); ++i)
                os << " ";

            // underline the error
            for (std::size_t i = col_start; i <= col_end; ++i)
                os << "^";

            os << "\n";
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Expected at least one argument: filename [-debug]" << std::endl;
        return 1;
    }

    std::string filename(argv[1]);
    bool debug = argc >= 3 && std::string(argv[2]) == "-debug";

    std::ifstream stream(filename);
    if (!stream.is_open())
        std::cout << "Failed to open " << filename << '\n';
    else
    {
        std::string code((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        Parser parser(code, debug);
        try
        {
            parser.parse();
        }
        catch (const ParseError& e)
        {
            std::cout << "ERROR\n"
                      << e.what() << "\n";

            std::string escaped_symbol;
            switch (e.symbol.codepoint())
            {
                case '\n': escaped_symbol = "'\\n'"; break;
                case '\r': escaped_symbol = "'\\r'"; break;
                case '\t': escaped_symbol = "'\\t'"; break;
                case '\v': escaped_symbol = "'\\v'"; break;
                case '\0': escaped_symbol = "EOF"; break;
                case ' ': escaped_symbol = "' '"; break;
                default:
                    escaped_symbol = e.symbol.c_str();
            }
            // e.line + 1 because we start counting at 0 and every code editor line counts starts at 1
            std::cout << "At " << escaped_symbol << " @ " << (e.line + 1) << ":" << (e.col + 1) << std::endl;

            makeContext(std::cout, code, e.line, e.col, e.expr);
        }
    }

    return 0;
}
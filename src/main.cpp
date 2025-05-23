#include "analysers/chars.hpp"
#include "analysers/symbols.hpp"
#include "specifications.hpp"

#include <iostream>

#include "oxygen/outcome.hpp"

int main(int argc, char **argv)
{
    Specifications specs;
    {
        xgn::Outcome oc = specs.generate(argc, argv);

        if (!oc.is_ok())
        {
            std::cout << oc.fatal_message();
            return 1;
        }

        if (specs.was_help())
            return 0;
    }

    analyser::CharsAnalyser chars_analyser;
    analyser::SymbolsAnalyser symbols_analyser;

    std::vector<analyser::ValidChar> chars;
    std::vector<analyser::Symbol> symbols;

    {
        auto oc =
            chars_analyser.analyse(specs.get_input_folder() + "chars.ebnf");

        if (!oc.is_ok())
        {
            std::cout << oc.outcome().fatal_message();
            return 1;
        }

        chars = *oc;
    }

    {
        auto oc = symbols_analyser.analyse(chars);

        if (!oc.is_ok())
        {
            std::cout << oc.outcome().fatal_message();
            return 1;
        }

        symbols = *oc;
    }

    for (auto symbol : symbols)
    {
        std::cout << "'" << symbol.value << "'\n";
    }

    return 0;
}
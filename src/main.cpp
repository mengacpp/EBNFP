#include "analysers/chars.hpp"
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

    analyser::CharsAnalyser char_analyser;

    std::vector<analyser::ValidChar> chars;

    {
        auto oc =
            char_analyser.analyse(specs.get_input_folder() + "chars.ebnf");

        if (!oc.is_ok())
        {
            std::cout << oc.outcome().fatal_message();
            return 1;
        }

        chars = *oc;
    }

    for (auto c : chars)
    {
        std::cout << c.value << "\n";
    }

    return 0;
}
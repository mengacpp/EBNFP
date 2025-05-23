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

    return 0;
}
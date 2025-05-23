#include "oxygen/log.hpp"
#include <iostream>

int main()
{
    std::cout << (xgn::log::Message() << "Hello, world!");

    return 0;
}
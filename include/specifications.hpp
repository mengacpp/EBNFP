#pragma once

#include <string>

#include "oxygen/outcome.hpp"

class Specifications
{
public:
    xgn::Outcome generate(int argc, char **argv);

    inline std::string get_input_folder() const { return m_input_folder; }
    inline std::string get_output_folder() const { return m_output_folder; }
    inline std::string get_program_name() const { return m_program_name; }

    inline bool was_help() const { return m_was_help; }

private:
    void print_usage();

    bool m_was_help;

    std::string m_program_name;
    std::string m_input_folder;
    std::string m_output_folder;
};

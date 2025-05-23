#include "specifications.hpp"

#include <iostream>

#include "oxygen/file.hpp"

std::string shift_arguments(int *argc, char ***argv)
{
    (*argc)--;
    return (*argv)++[0]; // Save the first argument
}

#define shift shift_arguments(&argc, &argv)

xgn::Outcome Specifications::generate(int argc, char **argv)
{
    m_program_name = shift;

    while (argc > 0)
    {
        std::string arg = shift;

        if (arg == "-h" || arg == "--help")
        {
            print_usage();
            m_was_help = true;
            return xgn::Outcome::ok();
        }
        else if (arg == "-o" || arg == "--output")
        {
            if (argc < 1)
            {
                return xgn::Outcome(xgn::log::Message()
                                    << "flag '" << arg
                                    << "' requires an argument");
            }

            if (!m_output_folder.empty())
            {
                return xgn::Outcome("output folder can't be defined twice");
            }

            m_output_folder = shift;
        }
        else
        {
            if (!m_input_folder.empty())
            {
                return xgn::Outcome("input folder can't be defined twice");
            }
            m_input_folder = arg;
        }
    }

    if (m_input_folder.empty())
        m_input_folder = "./";

    if (m_output_folder.empty())
        m_output_folder = "ebnf/out/";

    xgn::Outcome oc = xgn::file::create_dirs(m_output_folder);
    oc.update(xgn::file::is_dir(m_input_folder));
    oc.update(xgn::file::is_dir(m_output_folder));

    if (!oc.is_ok())
        return oc;

    return xgn::Outcome::ok();
}

void Specifications::print_usage()
{
    std::cout << "Usage\n"
              << m_program_name <<
        R"( <input_folder> [options]
Options:
    -o, --output <output_folder>    input the output folder   
    -h, --help                      print this message
)";
}
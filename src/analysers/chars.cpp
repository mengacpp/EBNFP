#include "analysers/chars.hpp"

#include "oxygen/file.hpp"
#include "oxygen/log.hpp"

namespace analyser
{
xgn::OutcomeOr<std::vector<ValidChar>> CharsAnalyser::analyse(std::string path)
{
    std::vector<ValidChar> valid_chars;
#define add_char(t, v) valid_chars.emplace_back(ValidCharType::t, v)

    xgn::Outcome oc = start_analysis(path);
    if (!oc.is_ok())
        return oc;

    while (peek() != '\0')
    {
        if (peek() == ';' || peek() == '.')
        {
            add_char(Sym_Terminator, eat());
            continue;
        }
        if (std::isalpha(peek()))
        {
            add_char(Letter, eat());
            continue;
        }
        if (std::isdigit(peek()))
        {
            add_char(DecimalDigit, eat());
            continue;
        }
        if (peek() == '\'')
        {
            add_char(Sym_QuoteFirst, eat());
            continue;
        }
        if (peek() == '"')
        {
            add_char(Sym_QuoteSecond, eat());
            continue;
        }
        if (peek() == ',')
        {
            add_char(Sym_Concat, eat());
            continue;
        }
        if (peek() == '=')
        {
            add_char(Sym_Define, eat());
            continue;
        }

        if ((peek() == ']') || (peek() == '/' && peek() == ')'))
        {
            std::string val = "]";

            if (peek() == '/')
            {
                eat();
                val = "/)";
            }

            eat();

            add_char(Sym_OptionEnd, val);
            continue;
        }

        if (peek() == '|' || peek() == '/' || peek() == '!')
        {
            add_char(Sym_DefinitionSeparator, eat());
            continue;
        }
        if (peek() == '-')
        {
            add_char(Sym_Except, eat());
            continue;
        }

        if (peek() == '*' && peek(1) == ')')
        {
            eat();
            eat();
            add_char(Sym_CommentEnd, "*)");
            continue;
        }
        if (peek() == '*')
        {
            add_char(Sym_Repetition, eat());
            continue;
        }

        if (peek() == '(' && peek(1) == '*')
        {
            eat();
            eat();
            add_char(Sym_CommentStart, "(*");
            continue;
        }

        if ((peek() == '{') || (peek() == '(' && peek(1) == ':'))
        {
            std::string val = "{";

            if (peek() == '(')
            {
                eat();
                val = "(:";
            }

            eat();

            add_char(Sym_RepeatStart, val);
            continue;
        }
        if ((peek() == '}') || (peek() == ':' && peek() == ')'))
        {
            std::string val = "}";

            if (peek() == ':')
            {
                eat();
                val = ":)";
            }

            eat();

            add_char(Sym_RepeatEnd, val);
            continue;
        }

        if ((peek() == '[') || (peek() == '(' && peek(1) == '/'))
        {
            std::string val = "[";

            if (peek() == '(')
            {
                eat();
                val = "(/";
            }

            eat();

            add_char(Sym_OptionStart, val);
            continue;
        }

        if (peek() == '(')
        {
            add_char(Sym_GroupStart, eat());
            continue;
        }

        if (peek() == ')')
        {
            add_char(Sym_GroupEnd, eat());
            continue;
        }

        if (peek() == '?')
        {
            add_char(Sym_SpecialSequence, eat());
            continue;
        }

        if (peek() == ':' || peek() == '+' || peek() == '_' || peek() == '%' ||
            peek() == '@' || peek() == '&' || peek() == '#' || peek() == '$' ||
            peek() == '<' || peek() == '>' || peek() == '\\' || peek() == '^' ||
            peek() == '`' || peek() == '~')
        {
            add_char(OtherChar, eat());
            continue;
        }

        if (peek() == ' ')
        {
            add_char(SpaceChar, eat());
            continue;
        }

        if (peek() == '\t')
        {
            add_char(HorizontalTabChar, eat());
            continue;
        }

        if (peek() == '\n')
        {
            add_char(NewLineChar, eat());
            continue;
        }

        if (peek() == '\v')
        {
            add_char(VerticalTabChar, eat());
            continue;
        }

        if (peek() == '\f')
        {
            add_char(FormFeed, eat());
            continue;
        }

        return xgn::Outcome(xgn::log::Message()
                            << get_current_pos() << ": invalid character '"
                            << peek() << "'");
    }

    return valid_chars;

#undef add_char
}

xgn::Outcome CharsAnalyser::start_analysis(std::string path)
{
    {
        auto oc = xgn::file::read_all(path);
        if (!oc.is_ok())
            return oc.outcome();

        m_source = *oc;
    }

    m_current_char = 0;
    m_current_ln = 1;
    m_current_lnc = 1;
    m_path = path;

    return xgn::Outcome::ok();
}

std::string CharsAnalyser::get_current_pos()
{
    return xgn::log::Message()
           << m_path << ":" << m_current_ln << ":" << m_current_lnc;
}

char CharsAnalyser::peek(int depth)
{
    int target = m_current_char + depth;
    if (target < 0 || target >= m_source.length())
        return '\0';

    return m_source.at(target);
}
char CharsAnalyser::eat()
{
    char c = m_source.at(m_current_char++);

    if (c == '\n')
    {
        m_current_ln++;
        m_current_lnc = 1;
    }
    else
    {
        m_current_lnc++;
    }

    return c;
}

}